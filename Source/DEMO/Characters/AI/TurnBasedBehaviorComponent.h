#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GameplayTagContainer.h"
#include "TurnBasedBehaviorComponent.generated.h"

class UBlackboardComponent;
class ATurnBasedCharacter;

UENUM(BlueprintType)
enum class EBehaviorType : uint8
{
	NotMyTurn, Wait, SelectSkill, SelectTarget, SkillSequence, MAX
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class DEMO_API UTurnBasedBehaviorComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UTurnBasedBehaviorComponent();
protected:
	virtual void BeginPlay() override;
public:
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	//property
private:
	UBlackboardComponent* Blackboard;
protected:
	UPROPERTY(EditAnywhere, Category = "BlackboardKey")
		FName BehaviorTypeKeyName = FName("BehaviorType");
public:

	//function
private:
	void ChangeType(EBehaviorType InNewType);

protected:
public:
	void SetNotMyTurnMode();
	void SetWaitMode();
	void SetSelectSkillMode();
	void SetSelectTargetMode();
	void SetSkillSequenceMode();

	bool IsNotMyTurnMode();
	bool IsWaitMode();
	bool IsSelectSkillMode();
	bool IsSelectTargetMode();
	bool IsSkillSequenceMode();

	FORCEINLINE void SetBlackboard(class UBlackboardComponent* InBlackboard) { Blackboard = InBlackboard; }
};
