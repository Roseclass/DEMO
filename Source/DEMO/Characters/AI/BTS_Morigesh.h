#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTService.h"
#include "BTS_Morigesh.generated.h"

/**
 * 
 */

class ATurnBasedPhaseManager;
class UBehaviorTreeComponent;
class UAbilityComponent;

UCLASS()
class DEMO_API UBTS_Morigesh : public UBTService
{
	GENERATED_BODY()
public:
	UBTS_Morigesh();
protected:
	virtual void OnBecomeRelevant(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

	//property
private:
protected:
	UPROPERTY(EditAnywhere)
		FBlackboardKeySelector BehaviorType;

	UPROPERTY(EditAnywhere, Category = "Character")
		FBlackboardKeySelector Data;
public:

	//function
private:
	void EvaluateTurnAction(UBehaviorTreeComponent& OwnerComp);
protected:
public:
};
