#include "Characters/AI/BTT_SelectTarget.h"
#include "Global.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/BTFunctionLibrary.h"

#include "Characters/AI/BlackboardTypes.h"
#include "Characters/TurnBasedCharacter.h"
#include "Characters/AI/TurnBasedEnemy.h"
#include "Characters/AI/TurnBasedAIController.h"
#include "Characters/AI/TurnBasedBehaviorComponent.h"

#include "GameAbilities/AbilityComponent.h"

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////UBTT_SelectTarget_Hover///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

UBTT_SelectTarget_Hover::UBTT_SelectTarget_Hover()
{
	NodeName = "SelectTarget_Hover";

	Data.AddObjectFilter(this, GET_MEMBER_NAME_CHECKED(UBTT_SelectTarget_Hover, Data), UTurnBasedBlackboardContainer::StaticClass());
}

EBTNodeResult::Type UBTT_SelectTarget_Hover::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::ExecuteTask(OwnerComp, NodeMemory);

	UBlackboardComponent* blackboardComp = OwnerComp.GetBlackboardComponent();
	if (!blackboardComp)
	{
		CLog::Print("UBTT_SelectTarget_Hover::ExecuteTask blackboardComp cast failed!!",-1,0);
		return EBTNodeResult::Failed;
	}

	AAIController* controller = Cast<AAIController>(OwnerComp.GetOwner());
	if (!controller)
	{
		CLog::Print("UBTT_SelectTarget_Hover::ExecuteTask controller cast failed!!", -1, 0);
		return EBTNodeResult::Failed;
	}

	ATurnBasedEnemy* aiPawn = Cast<ATurnBasedEnemy>(controller->GetPawn());
	if (!aiPawn)
	{
		CLog::Print("UBTT_SelectTarget_Hover::ExecuteTask aiPawn cast failed!!", -1, 0);
		return EBTNodeResult::Failed;
	}

	UTurnBasedBlackboardContainer* data = Cast<UTurnBasedBlackboardContainer>(blackboardComp->GetValueAsObject(Data.SelectedKeyName));
	if (!data)
	{
		CLog::Print("UBTT_SelectTarget_Hover::ExecuteTask data cast failed!!", -1, 0);
		return EBTNodeResult::Failed;
	}

	ATurnBasedCharacter* target = nullptr;
	for (int32 i = 0; i < data->SkillTargets.Num(); i++)
	{
		if (!data->SelectedTargets.IsValidIndex(i))
			target = data->SkillTargets[i];
	}
	
	if (target)
	{
		data->SelectedTargets.Add(target);
		aiPawn->HoverTarget(target);
	}
	else
	{
		CLog::Print("UBTT_SelectTarget_Hover::ExecuteTask target is nullptr!!", -1, 0);
		return EBTNodeResult::Failed;
	}

	return EBTNodeResult::Succeeded;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////UBTT_SelectTarget_Hover///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

UBTT_SelectTarget_Click::UBTT_SelectTarget_Click()
{
	NodeName = "SelectTarget_Click";

	Data.AddObjectFilter(this, GET_MEMBER_NAME_CHECKED(UBTT_SelectTarget_Click, Data), UTurnBasedBlackboardContainer::StaticClass());
}

EBTNodeResult::Type UBTT_SelectTarget_Click::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::ExecuteTask(OwnerComp, NodeMemory);

	UBlackboardComponent* blackboardComp = OwnerComp.GetBlackboardComponent();
	if (!blackboardComp)
	{
		CLog::Print("UBTT_SelectTarget_Click::ExecuteTask blackboardComp cast failed!!", -1, 0);
		return EBTNodeResult::Failed;
	}

	AAIController* controller = Cast<AAIController>(OwnerComp.GetOwner());
	if (!controller)
	{
		CLog::Print("UBTT_SelectTarget_Click::ExecuteTask controller cast failed!!", -1, 0);
		return EBTNodeResult::Failed;
	}

	ATurnBasedEnemy* aiPawn = Cast<ATurnBasedEnemy>(controller->GetPawn());
	if (!aiPawn)
	{
		CLog::Print("UBTT_SelectTarget_Click::ExecuteTask aiPawn cast failed!!", -1, 0);
		return EBTNodeResult::Failed;
	}

	UTurnBasedBlackboardContainer* data = Cast<UTurnBasedBlackboardContainer>(blackboardComp->GetValueAsObject(Data.SelectedKeyName));
	if (!data)
	{
		CLog::Print("UBTT_SelectTarget_Click::ExecuteTask data cast failed!!", -1, 0);
		return EBTNodeResult::Failed;
	}

	aiPawn->ClickTarget(data->SelectedTargets[data->SelectedTargets.Num() - 1]);

	return EBTNodeResult::Succeeded;
}

/*
* 
* 스킬 호버->선택->타겟뷰->타겟호버<->타겟클릭->playsequence
* 
* 일단 링크는 다했음
* widget activate확인
* 
* 본인턴아닐때 마우스 호버 끄기
* 
*/