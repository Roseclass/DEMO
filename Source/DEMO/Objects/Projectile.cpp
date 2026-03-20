#include "Objects/Projectile.h"
#include "Global.h"
#include "Components/ShapeComponent.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "AbilitySystemInterface.h"
#include "AbilitySystemGlobals.h"
#include "GameplayEffect.h"
#include "GameplayEffectExecutionCalculation.h"
#include "GameplayCueManager.h"

#include "Characters/TurnBasedCharacter.h"

#include "GameAbilities/AbilityComponent.h"
#include "GameAbilities/AttributeSet_Character.h"
#include "GameAbilities/GameplayEffectContexts.h"


AProjectile::AProjectile()
{
	PrimaryActorTick.bCanEverTick = true;

	CHelpers::CreateActorComponent<UProjectileMovementComponent>(this, &ProjectileMovementComponent, "Projectile");
}

void AProjectile::BeginPlay()
{
	Super::BeginPlay();
	FindCollision();
}

void AProjectile::Tick(float DeltaTime)
{
	AActor::Tick(DeltaTime);

	if (bCollsion)
	{
		CurrentDamageTick += DeltaTime;
		if (CurrentDamageTick < DamageTick)
		{
			DamageTick -= CurrentDamageTick;
			SendDamage(GetTarget(), HitResult);
		}
	}
}

void AProjectile::OnComponentBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	// overlap with DungeonCharacterBase
	ATurnBasedCharacter* other = Cast<ATurnBasedCharacter>(OtherActor);
	CheckTrue(!other);
	CheckTrue(other != GetDataContext().TargetActor);

	HitResult = SweepResult;

	bCollsion = 1;
	SendEvent(GetDataContext().CollisionTriggerDatas, bCollsionEvent);
	SendDamage(OtherActor, SweepResult);
}

void AProjectile::OnComponentEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	bCollsion = 0;
}

void AProjectile::FindCollision()
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
		component->OnComponentBeginOverlap.AddDynamic(this, &AProjectile::OnComponentBeginOverlap);
		component->OnComponentEndOverlap.AddDynamic(this, &AProjectile::OnComponentEndOverlap);
	}
}

bool AProjectile::TryDestroy()
{
	bool result = Super::TryDestroy();

	if (result)
	{
		/*
		* destroy particle
		*/
	}

	return result;
}

void AProjectile::Init(const FSpawnDamageDealerContext* InData)
{
	Super::Init(InData);

	bCollsionEvent = !bUseCollsionEvent;
}

AActor* AProjectile::GetTarget()
{
	return GetDataContext().TargetActor.Get();
}
