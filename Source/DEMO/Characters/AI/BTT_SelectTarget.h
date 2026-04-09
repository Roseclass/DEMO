#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTT_SelectTarget.generated.h"

/**
 * 
 */
UCLASS()
class DEMO_API UBTT_SelectTarget_Hover : public UBTTaskNode
{
	GENERATED_BODY()
public:
	UBTT_SelectTarget_Hover();
protected:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

	//property
private:
protected:
	UPROPERTY(EditAnywhere, Category = "Character")
		FBlackboardKeySelector Data;
public:
};

UCLASS()
class DEMO_API UBTT_SelectTarget_Click : public UBTTaskNode
{
	GENERATED_BODY()
public:
	UBTT_SelectTarget_Click();
protected:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

	//property
private:
protected:
	UPROPERTY(EditAnywhere, Category = "Character")
		FBlackboardKeySelector Data;
public:
};
