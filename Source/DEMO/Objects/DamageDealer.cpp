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
#include "GameAbilities/GameplayEffectContexts.h"
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
}

void ADamageDealer::SendDamage(TSubclassOf<UGameplayEffect> EffectClass, AActor* Target, const FHitResult& SweepResult)
{
	CheckTrue(MaxHitCount <= CurrentHitCount);

	SendEvent(GetDataContext().DamageSendTriggerDatas, bSendDamageEvent);

	IAbilitySystemInterface* hitCharacter = Cast<IAbilitySystemInterface>(Target);
	if (hitCharacter)
	{
		// Get asc
		ATurnBasedCharacter* owner = Cast<ATurnBasedCharacter>(GetOwner());
		UAbilityComponent* hitASC = Cast<UAbilityComponent>(hitCharacter->GetAbilitySystemComponent());
		UAbilityComponent* instigatorASC = Cast<UAbilityComponent>(owner->GetAbilitySystemComponent());
		if (hitASC && instigatorASC && EffectClass)
		{
			// Make effectcontext handle
			FDamageEffectContext* context = new FDamageEffectContext();
			context->BaseDamage = CalculateDamage(instigatorASC->GetPower());
			context->AddOrigin(Target->GetActorLocation());

			FGameplayEffectContextHandle EffectContextHandle = FGameplayEffectContextHandle(context);
			EffectContextHandle.AddInstigator(owner ? owner->GetController() : nullptr, this);
			EffectContextHandle.AddHitResult(SweepResult);

			// Must use EffectToTarget for auto mmc
			instigatorASC->ApplyGameplayEffectToTarget(EffectClass.GetDefaultObject(), hitASC, UGameplayEffect::INVALID_LEVEL, EffectContextHandle);
		}
	}
	if (MaxHitCount <= ++CurrentHitCount)
		Deactivate();
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
				instigator->GetAbilitySystemComponent()->HandleGameplayEvent(i.Tag, &data);
			});
		if (i.Delay <= 1e-9)
		{
			func.Execute();
			continue;
		}
		FTimerHandle spawnNextDamageDealerHandle;
		GetWorld()->GetTimerManager().SetTimer(spawnNextDamageDealerHandle, func, i.Delay, false);
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