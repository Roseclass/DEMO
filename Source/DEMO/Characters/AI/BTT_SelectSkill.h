#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTT_SelectSkill.generated.h"

/**
 * 
 */

UCLASS()
class DEMO_API UBTT_SelectSkill_Hover : public UBTTaskNode
{
	GENERATED_BODY()
public:
	UBTT_SelectSkill_Hover();
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
class DEMO_API UBTT_SelectSkill_Click : public UBTTaskNode
{
	GENERATED_BODY()
public:
	UBTT_SelectSkill_Click();
protected:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

	//property
private:
protected:
	UPROPERTY(EditAnywhere, Category = "Character")
		FBlackboardKeySelector Data;
public:
};
