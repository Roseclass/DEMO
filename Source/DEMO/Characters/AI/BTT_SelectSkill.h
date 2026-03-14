#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTT_SelectSkill.generated.h"

/**
 * 
 */

UCLASS()
class DEMO_API UBTT_SelectSkill : public UBTTaskNode
{
	GENERATED_BODY()
public:
	UBTT_SelectSkill();
protected:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual FString GetStaticDescription() const override;

	//property
private:
protected:
	UPROPERTY(EditAnywhere, Category = "SkillTag")
		FBlackboardKeySelector SelectedSkillTag;

	UPROPERTY(EditAnywhere, Category = "SkillTag")
		FBlackboardKeySelector TargetSkillTag;
public:

	//function
private:
protected:
public:

};
