#include "Objects/EventTrigger.h"
#include "Global.h"
#include "Components/ShapeComponent.h"

#include "Characters/BaseCharacter.h"

AEventTrigger::AEventTrigger()
{
	PrimaryActorTick.bCanEverTick = true;
}

void AEventTrigger::BeginPlay()
{
	Super::BeginPlay();		
	FindCollision();
}

void AEventTrigger::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AEventTrigger::OnComponentBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	CheckTrue(!bAct);

	CheckTrue_Print(!OnBeginOverlap.IsBound(), "OnBeginOverlap is empty");
	CheckTrue(OverlappedActors.Contains(OtherActor));

	ABaseCharacter* owner = Cast<ABaseCharacter>(GetOwner());
	CheckTrue_Print(!owner, "owner is not ABaseCharacter");

	ABaseCharacter* other = Cast<ABaseCharacter>(OtherActor);
	CheckTrue(!other);

	CheckTrue(other->GetGenericTeamId() == owner->GetGenericTeamId());

	OverlappedActors.AddUnique(other);
	CurrentOverlappedActor = other;

	FGameplayTag tag;
	FGameplayEventData* data = new FGameplayEventData();

	data->Instigator = owner;
	data->Target = other;

	OnBeginOverlap.Broadcast(tag, data);
}

void AEventTrigger::OnComponentEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	//CheckTrue_Print(!OnEndOverlap.IsBound(), "OnEndOverlap is empty");
	//OnEndOverlap.Broadcast();
}

void AEventTrigger::FindCollision()
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
		component->OnComponentBeginOverlap.AddDynamic(this, &AEventTrigger::OnComponentBeginOverlap);
		component->OnComponentEndOverlap.AddDynamic(this, &AEventTrigger::OnComponentEndOverlap);
	}
}

void AEventTrigger::Activate()
{
	bAct = 1;
}

void AEventTrigger::Deactivate()
{
	bAct = 0;
	OverlappedActors.Empty();
}
