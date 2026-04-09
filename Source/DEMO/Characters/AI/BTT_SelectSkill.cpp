#include "Characters/AI/BTT_SelectSkill.h"
#include "Global.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/BTFunctionLibrary.h"

#include "Characters/TurnBasedCharacter.h"

#include "Characters/AI/BlackboardTypes.h"
#include "Characters/AI/TurnBasedEnemy.h"
#include "Characters/AI/TurnBasedAIController.h"
#include "Characters/AI/TurnBasedBehaviorComponent.h"

#include "GameAbilities/AbilityComponent.h"

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////UBTT_SelectSkill_Hover///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

UBTT_SelectSkill_Hover::UBTT_SelectSkill_Hover()
{
	NodeName = "SelectSkill_Hover";

	Data.AddObjectFilter(this, GET_MEMBER_NAME_CHECKED(UBTT_SelectSkill_Hover, Data), UTurnBasedBlackboardContainer::StaticClass());
}

EBTNodeResult::Type UBTT_SelectSkill_Hover::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::ExecuteTask(OwnerComp, NodeMemory);

	UBlackboardComponent* blackboardComp = OwnerComp.GetBlackboardComponent();
	if (!blackboardComp)
	{
		CLog::Print("UBTT_SelectSkill_Hover::ExecuteTask blackboardComp cast failed!!");
		return EBTNodeResult::Failed;
	}

	AAIController* controller = Cast<AAIController>(OwnerComp.GetOwner());
	if (!controller)
	{
		CLog::Print("UBTT_SelectSkill_Hover::ExecuteTask controller cast failed!!");
		return EBTNodeResult::Failed;
	}

	ATurnBasedEnemy* aiPawn = Cast<ATurnBasedEnemy>(controller->GetPawn());
	if (!aiPawn)
	{
		CLog::Print("UBTT_SelectSkill_Hover::ExecuteTask aiPawn cast failed!!");
		return EBTNodeResult::Failed;
	}

	UTurnBasedBlackboardContainer* data = Cast<UTurnBasedBlackboardContainer>(blackboardComp->GetValueAsObject(Data.SelectedKeyName));
	if (!data)
	{
		CLog::Print("UBTT_SelectSkill_Hover::ExecuteTask data cast failed!!");
		return EBTNodeResult::Failed;
	}

	aiPawn->HoverSkillIcon(data->TargetSkillTag);

	return EBTNodeResult::Succeeded;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////UBTT_SelectSkill_Click///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

UBTT_SelectSkill_Click::UBTT_SelectSkill_Click()
{
	NodeName = "SelectSkill_Click";

	Data.AddObjectFilter(this, GET_MEMBER_NAME_CHECKED(UBTT_SelectSkill_Click, Data), UTurnBasedBlackboardContainer::StaticClass());
}

EBTNodeResult::Type UBTT_SelectSkill_Click::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::ExecuteTask(OwnerComp, NodeMemory);

	UBlackboardComponent* blackboardComp = OwnerComp.GetBlackboardComponent();
	if (!blackboardComp)
	{
		CLog::Print("UBTT_SelectSkill_Click::ExecuteTask blackboardComp cast failed!!");
		return EBTNodeResult::Failed;
	}

	AAIController* controller = Cast<AAIController>(OwnerComp.GetOwner());
	if (!controller)
	{
		CLog::Print("UBTT_SelectSkill_Click::ExecuteTask controller cast failed!!");
		return EBTNodeResult::Failed;
	}

	ATurnBasedEnemy* aiPawn = Cast<ATurnBasedEnemy>(controller->GetPawn());
	if (!aiPawn)
	{
		CLog::Print("UBTT_SelectSkill_Click::ExecuteTask aiPawn cast failed!!");
		return EBTNodeResult::Failed;
	}

	UTurnBasedBlackboardContainer* data = Cast<UTurnBasedBlackboardContainer>(blackboardComp->GetValueAsObject(Data.SelectedKeyName));
	if (!data)
	{
		CLog::Print("UBTT_SelectSkill_Click::ExecuteTask data cast failed!!");
		return EBTNodeResult::Failed;
	}

	aiPawn->ClickSkillIcon(data->TargetSkillTag);

	return EBTNodeResult::Succeeded;
}
