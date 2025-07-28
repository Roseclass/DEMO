#include "DamageDealer.h"
#include "Global.h"
#include "Components/ShapeComponent.h"
#include "GameFramework/PlayerController.h"
#include "AbilitySystemInterface.h"
#include "AbilitySystemGlobals.h"
#include "GameplayEffect.h"
#include "GameplayEffectExecutionCalculation.h"
#include "GameplayCueManager.h"

#include "DEMOCharacter.h"
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
	FindCollision();
}

void ADamageDealer::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ADamageDealer::OnComponentBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	CheckFalse(HasAuthority());

	// already hit?
	if (GetDamagedActors().Contains(OtherActor))return;

	// overlap with DungeonCharacterBase
	ADEMOCharacter* base = Cast<ADEMOCharacter>(OtherActor);
	if (!base)return;

	// is deadmode?
	// UNDONE::이게 꼭 필요할까? 턴제라 다단히트 맞으면 중간에 죽을수도있지 그럼 시퀀스 안터트릴거야 ?
	//UAbilityComponent* gasComp = CHelpers::GetComponent<UAbilityComponent>(OtherActor);
	//if (!gasComp)return;
	//if (gasComp->IsDead())return;

	// ignore alliance
	CheckTrue(base->GetGenericTeamId() == TeamID);

	// set properties
	OverlappedActors.AddUnique(OtherActor);
	CurrentOverlappedActor = OtherActor;

	// send Damage
	if (OtherActor && OtherActor != this)
		SendDamage(GamePlayEffectClass, OtherActor, SweepResult);
}

void ADamageDealer::OnComponentEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	CheckFalse(HasAuthority());

	// set properties
	OverlappedActors.Remove(OtherActor);
	if (CurrentOverlappedActor == OtherActor && OverlappedActors.Num())
		CurrentOverlappedActor = OverlappedActors.Last();
}

void ADamageDealer::ResetDamagedActors()
{
	// reset property
	DamagedActors.Empty();
}

void ADamageDealer::FindCollision()
{
	TArray<UShapeComponent*> shapeComponents;
	GetComponents<UShapeComponent>(shapeComponents);
	for (UShapeComponent* component : shapeComponents)
	{
		for (auto i : component->ComponentTags)
		{
			if (i == OverlapComponentTag)
			{
				CollisionComponents.Add(component);
				break;
			}
		}
	}
	for (UShapeComponent* component : CollisionComponents)
	{
		component->OnComponentBeginOverlap.Clear();
		component->OnComponentEndOverlap.Clear();
		component->OnComponentBeginOverlap.AddDynamic(this, &ADamageDealer::OnComponentBeginOverlap);
		component->OnComponentEndOverlap.AddDynamic(this, &ADamageDealer::OnComponentEndOverlap);
	}
}

void ADamageDealer::SendDamage(TSubclassOf<UGameplayEffect> EffectClass, AActor* Target, const FHitResult& SweepResult)
{
	// TEST::데미지텍스트 트리거
	IAbilitySystemInterface* hitCharacter = Cast<IAbilitySystemInterface>(Target);
	if (hitCharacter)
	{
		// Get asc
		ADEMOCharacter* owner = Cast<ADEMOCharacter>(GetOwner());
		UAbilityComponent* hitASC = Cast<UAbilityComponent>(hitCharacter->GetAbilitySystemComponent());
		UAbilityComponent* instigatorASC = Cast<UAbilityComponent>(owner->GetAbilitySystemComponent());
		if (hitASC && instigatorASC && EffectClass)
		{
			// Make effectcontext handle
			FDamageEffectContext* context = new FDamageEffectContext();
			FGameplayEffectContextHandle EffectContextHandle = FGameplayEffectContextHandle(context);
			EffectContextHandle.AddInstigator(owner ? owner->GetController() : nullptr, this);
			EffectContextHandle.AddHitResult(SweepResult);
			context->BaseDamage = CalculateDamage(instigatorASC->GetPower());

			// Must use EffectToTarget for auto mmc
			instigatorASC->ApplyGameplayEffectToTarget(EffectClass.GetDefaultObject(), hitASC, UGameplayEffect::INVALID_LEVEL, EffectContextHandle);
		}
		CurrentDamagedActor = Target;
	}
}

void ADamageDealer::Activate()
{
	bAct = 1;
}

void ADamageDealer::Deactivate()
{
	bAct = 0;
}