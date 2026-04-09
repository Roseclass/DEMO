#include "Characters/AI/BTT_ChangeBehaviorType.h"
#include "Global.h"

#include "Characters/AI/TurnBasedAIController.h"
#include "Characters/AI/TurnBasedBehaviorComponent.h"


UBTT_ChangeBehaviorType::UBTT_ChangeBehaviorType()
{
	NodeName = "ChangeBehaviorType";
}

EBTNodeResult::Type UBTT_ChangeBehaviorType::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::ExecuteTask(OwnerComp, NodeMemory);

	AAIController* controller = Cast<AAIController>(OwnerComp.GetOwner());
	if (!controller)
	{
		CLog::Print("UBTT_ChangeBehaviorState::ExecuteTask controller cast failed!!");
		return EBTNodeResult::Failed;
	}

	UTurnBasedBehaviorComponent* behavior = CHelpers::GetComponent<UTurnBasedBehaviorComponent>(controller);
	if (!behavior)
	{
		CLog::Print("UBTT_ChangeBehaviorState::ExecuteTask behavior cast failed!!");
		return EBTNodeResult::Failed;
	}

	switch (BehaviorType)
	{
	case EBehaviorType::NotMyTurn:behavior->SetNotMyTurnMode(); break;
	case EBehaviorType::Wait:behavior->SetWaitMode(); break;
	case EBehaviorType::SelectSkill:behavior->SetSelectSkillMode(); break;
	case EBehaviorType::SelectTarget:behavior->SetSelectTargetMode(); break;
	case EBehaviorType::SkillSequence:behavior->SetSelectSkillMode(); break;
	default:break;
	}
	

	return EBTNodeResult::Succeeded;
}
