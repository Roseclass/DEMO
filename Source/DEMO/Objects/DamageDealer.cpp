#include "Objects/DamageDealer.h"
#include "Global.h"
#include "Components/ShapeComponent.h"
#include "GameFramework/PlayerController.h"
#include "AbilitySystemInterface.h"
#include "AbilitySystemGlobals.h"
#include "GameplayEffect.h"
#include "GameplayEffectExecutionCalculation.h"
#include "GameplayCueManager.h"

#include "Characters/TurnBasedCharacter.h"

#include "GameAbilities/AbilityComponent.h"
#include "GameAbilities/AttributeSet_Character.h"
#include "GameAbilities/ExecutionContextTypes.h"
#include "GameAbilities/GameplayEffectContexts.h"
#include "GameAbilities/GE_InstantDamage.h"
#include "GameAbilities/MMC_Damage.h"

ADamageDealer::ADamageDealer()
{
	PrimaryActorTick.bCanEverTick = true;
}
void ADamageDealer::BeginPlay()
{
	Super::BeginPlay();
	Activate();
}

void ADamageDealer::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bAct)
	{
		CurrentDamageTick += DeltaTime;
		if (CurrentDamageTick < DamageTick)
		{
			DamageTick -= CurrentDamageTick;
			SendDamage(GetDataContext().TargetActor.Get(), HitResult);
		}
	}
}

void ADamageDealer::SendDamage(AActor* Target, const FHitResult& SweepResult)
{
	ABaseCharacter* hitCharacter = Cast<ABaseCharacter>(Target);
	CheckNull(hitCharacter);
	CheckTrue(MaxHitCount <= CurrentHitCount);

	// Get asc
	ABaseCharacter* owner = Cast<ABaseCharacter>(GetOwner());
	UAbilityComponent* instigatorASC = Cast<UAbilityComponent>(owner->GetAbilitySystemComponent());
	UAbilityComponent* targetASC = Cast<UAbilityComponent>(hitCharacter->GetAbilitySystemComponent());
	ApplyDamageGE(instigatorASC, targetASC, SweepResult);
	ApplyAdditiveEffectData(instigatorASC, targetASC, SweepResult);
	SendEvent(GetDataContext().DamageSendTriggerDatas, bSendDamageEvent);

	if (MaxHitCount <= ++CurrentHitCount)
		Deactivate();
}

void ADamageDealer::ApplyDamageGE(UAbilityComponent* InstigatorASC, UAbilityComponent* TargetASC, const FHitResult& SweepResult)
{
	CheckTrue_Print(!DamageGEClass, "DamageGEClass is nullptr!!");
	if (InstigatorASC && TargetASC && DamageGEClass)
	{
		FExecutionContext* context = new FExecutionContext();
		context->EffectCauserActor = InstigatorASC->GetAvatarActor();
		context->EffectSourceActor = InstigatorASC->GetAvatarActor();
		context->EffectTargetActor = TargetASC->GetAvatarActor();
		context->SkillCauserActor = InstigatorASC->GetAvatarActor();
		context->SkillTargetActor = TargetASC->GetAvatarActor();

		FGameplayEffectContextHandle EffectContextHandle = FGameplayEffectContextHandle(context);
		EffectContextHandle.AddInstigator(InstigatorASC->GetAvatarActor(), this);
		EffectContextHandle.AddHitResult(SweepResult);

		FGameplayEffectSpecHandle EffectSpecHandle = InstigatorASC->MakeOutgoingSpec(DamageGEClass, 1, EffectContextHandle);

		InstigatorASC->ApplyGameplayEffectSpecToTarget(*EffectSpecHandle.Data, TargetASC);
	}
}

void ADamageDealer::ApplyAdditiveEffectData(UAbilityComponent* InstigatorASC, UAbilityComponent* TargetASC, const FHitResult& SweepResult)
{
	//TODO::데미지 따로 계산할수있게하기

	CheckTrue(AdditiveEffectData.Rule == EDamageTriggerRule::NONE);
	CheckTrue(AdditiveEffectData.Rule == EDamageTriggerRule::OnFirstHit && 0 < CurrentHitCount);
	if (InstigatorASC && TargetASC)
	{
		// Make effectcontext handle
		FDamageParameters params;
		params.InstigatorPower = InstigatorASC->GetPower();
		params.InstigatorSpeed = InstigatorASC->GetSpeed();
		params.TargetDefense = TargetASC->GetDefense();
		params.TargetHealth = TargetASC->GetHealth();
		params.TargetMaxHealth = TargetASC->GetMaxHealth();

		//float calculatedDamage = CalculateDamage(params);

		FDamageEffectContext* context = new FDamageEffectContext();
		//context->CalculatedDamage = calculatedDamage;
		context->Location = TargetASC->GetAvatarActor()->GetActorLocation();

		FGameplayEffectContextHandle EffectContextHandle = FGameplayEffectContextHandle(context);
		EffectContextHandle.AddInstigator(InstigatorASC->GetAvatarActor(), this);
		EffectContextHandle.AddHitResult(SweepResult);

		FGameplayEffectSpecHandle EffectSpecHandle = InstigatorASC->MakeOutgoingSpec(AdditiveEffectData.GEClass, 1, EffectContextHandle);
		EffectSpecHandle.Data.Get()->StackCount = AdditiveEffectData.StackCount;
		EffectSpecHandle.Data.Get()->DynamicGrantedTags.AddTag(AdditiveEffectData.GrantedTag);

		InstigatorASC->ApplyGameplayEffectSpecToTarget(*EffectSpecHandle.Data, TargetASC);
	}
}

void ADamageDealer::Activate()
{
	FTimerDelegate func =
		FTimerDelegate::CreateLambda([&]()
			{
				bAct = 1;
				SendEvent(GetDataContext().ActivateTriggerDatas, bActivateEvent);
			});
	if (ActivateDelay <= 1e-9)
	{
		func.Execute();
		return;
	}
	FTimerHandle activateHandle;
	GetWorld()->GetTimerManager().SetTimer(activateHandle, func, ActivateDelay, false);
}

void ADamageDealer::ApplyNextCameraMove()
{
	bApplyNextCameraMove = 1;
}

void ADamageDealer::PlayNextMontage()
{
	bPlayNextMontage = 1;
}

void ADamageDealer::SpawnNextDamageDealer()
{
	bSpawnNextDamageDealer = 1;
}

void ADamageDealer::Deactivate()
{
	SendEvent(GetDataContext().DeactivateTriggerDatas, bDeactivateEvent);

	//deactivate
	{
		FTimerDelegate func =
			FTimerDelegate::CreateLambda([&]()
				{
					bAct = 0;
					TryDestroy();
				});
		if (DeactivateDelay <= 1e-9)func.Execute();
		else
		{
			FTimerHandle handle;
			GetWorld()->GetTimerManager().SetTimer(handle, func, DeactivateDelay, false);
		}
	}

	//applyNextCameraMove
	{
		FTimerDelegate func =
			FTimerDelegate::CreateLambda([&]()
				{
					ApplyNextCameraMove();
					TryDestroy();
				});
		if (CameraMoveDelay <= 1e-9)func.Execute();
		else
		{
			FTimerHandle handle;
			GetWorld()->GetTimerManager().SetTimer(handle, func, CameraMoveDelay, false);
		}
	}

	//playNextMontage
	{
		FTimerDelegate func =
			FTimerDelegate::CreateLambda([&]()
				{
					PlayNextMontage();
					TryDestroy();
				});
		if (MontageDelay <= 1e-9)func.Execute();
		else
		{
			FTimerHandle handle;
			GetWorld()->GetTimerManager().SetTimer(handle, func, MontageDelay, false);
		}
	}

	//spawnNextDamageDealer
	{
		FTimerDelegate func =
			FTimerDelegate::CreateLambda([&]()
				{
					SpawnNextDamageDealer();
					TryDestroy();
				});
		if (SpawnDamageDealerDelay <= 1e-9)func.Execute();
		else
		{
			FTimerHandle handle;
			GetWorld()->GetTimerManager().SetTimer(handle, func, SpawnDamageDealerDelay, false);
		}
	}
}

bool ADamageDealer::TryDestroy()
{
	if (bAct)return 0;
	if (!bApplyNextCameraMove)return 0;
	if (!bPlayNextMontage)return 0;
	if (!bSpawnNextDamageDealer)return 0;
	Destroy();
	return 1;
}

void ADamageDealer::SendEvent(const TArray<FDamageDealerTriggerData>& InDatas, bool& InFlag)
{
	CheckTrue(InFlag);
	
	for (const auto& i : InDatas)
	{
		FTimerDelegate func = 
		FTimerDelegate::CreateLambda([&]()
			{
				ATurnBasedCharacter* instigator = Cast<ATurnBasedCharacter>(Data.GetInstigator());
				FGameplayEventData data;
				UAbilitySystemComponent* asc = instigator->GetAbilitySystemComponent();
				if (!asc)
				{
					CLog::Print(instigator->GetName() + i.Tag.ToString(), -1, 10, FColor::Purple);
					return;
				}
				asc->HandleGameplayEvent(i.Tag, &data);
			});
		if (i.Delay <= 1e-9)
		{
			func.Execute();
			continue;
		}
		FTimerHandle handle;
		GetWorld()->GetTimerManager().SetTimer(handle, func, i.Delay, false);
	}
	InFlag = 1;
}

void ADamageDealer::Init(const FSpawnDamageDealerContext* InData)
{
	bApplyNextCameraMove = !bUseDeactivateCameraMove;
	bPlayNextMontage = !bUseDeactivateMontage;
	bSpawnNextDamageDealer = !bUseDeactivateSpawnDamageDealer;

	Data = *InData;
}