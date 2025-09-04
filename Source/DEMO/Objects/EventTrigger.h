#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameplayTagContainer.h"
#include "Abilities/GameplayAbilityTypes.h"
#include "EventTrigger.generated.h"

DECLARE_MULTICAST_DELEGATE_TwoParams(FTriggerEventSignature, FGameplayTag, const FGameplayEventData*);

UCLASS()
class DEMO_API AEventTrigger : public AActor
{
	GENERATED_BODY()
	
public:	
	AEventTrigger();
protected:
	virtual void BeginPlay() override;
public:	
	virtual void Tick(float DeltaTime) override;

	//property
private:
	bool bAct;
	TArray<UShapeComponent*> CollisionComponents;

	TArray<AActor*> OverlappedActors;
	AActor* CurrentOverlappedActor;
protected:
	UPROPERTY(EditDefaultsOnly, Category = "Datas")
		FName OverlapComponentTag = FName("OverlapCollision");
public:
	FTriggerEventSignature OnBeginOverlap;
	FTriggerEventSignature OnEndOverlap;

	//function
private:
	UFUNCTION()void OnComponentBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()void OnComponentEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
	void FindCollision();
protected:
public:
	void Activate();
	void Deactivate();
	FORCEINLINE bool IsActivated()const { return bAct; }

};
