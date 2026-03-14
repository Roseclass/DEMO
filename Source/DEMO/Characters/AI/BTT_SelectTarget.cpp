#include "Characters/AI/BTT_SelectTarget.h"
#include "Global.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/BTFunctionLibrary.h"

#include "Characters/TurnBasedCharacter.h"
#include "Characters/AI/TurnBasedEnemy.h"
#include "Characters/AI/TurnBasedAIController.h"
#include "Characters/AI/TurnBasedBehaviorComponent.h"

#include "GameAbilities/AbilityComponent.h"

UBTT_SelectTarget::UBTT_SelectTarget()
{
	NodeName = "SelectTarget";

	SelectedCharacter.AddObjectFilter(this, GET_MEMBER_NAME_CHECKED(UBTT_SelectTarget, SelectedCharacter), ATurnBasedCharacter::StaticClass());
	TargetCharacter.AddObjectFilter(this, GET_MEMBER_NAME_CHECKED(UBTT_SelectTarget, TargetCharacter), ATurnBasedCharacter::StaticClass());
}

EBTNodeResult::Type UBTT_SelectTarget::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::ExecuteTask(OwnerComp, NodeMemory);

	UBlackboardComponent* blackboardComp = OwnerComp.GetBlackboardComponent();
	if (!blackboardComp)return EBTNodeResult::Failed;

	AAIController* controller = Cast<AAIController>(OwnerComp.GetOwner());
	UTurnBasedBehaviorComponent* behavior = CHelpers::GetComponent<UTurnBasedBehaviorComponent>(controller);

	ATurnBasedEnemy* aiPawn = Cast<ATurnBasedEnemy>(controller->GetPawn());
	UAbilityComponent* asc = CHelpers::GetComponent<UAbilityComponent>(aiPawn);

	TArray<ATurnBasedCharacter*> targetArray = aiPawn->GetTargetArray();
	int32 size = targetArray.Num();

	ATurnBasedCharacter* currentTarget = Cast<ATurnBasedCharacter>(blackboardComp->GetValueAsObject(SelectedCharacter.SelectedKeyName));
	ATurnBasedCharacter* targetTarget = Cast<ATurnBasedCharacter>(blackboardComp->GetValueAsObject(TargetCharacter.SelectedKeyName));

	if (!targetTarget)
	{
		CLog::Print("UBTT_SelectTarget::ExecuteTask Target is nullptr", -1, 10, FColor::Red);
		return EBTNodeResult::Failed;
	}

	if (currentTarget == targetTarget) return EBTNodeResult::Succeeded;

	int32 idx = -1;
	int32 tIdx = -1;

	for (int32 i = 0; i < size; i++)
	{
		if (targetArray[i] == currentTarget)idx = i;
		if (targetArray[i] == targetTarget)tIdx = i;
	}

	if (idx < 0)
	{
		CLog::Print("UBTT_SelectSkill::ExecuteTask currentTarget is nullptr", -1, 10, FColor::Red);
		return EBTNodeResult::Failed;
	}

	if (tIdx < 0)
	{
		CLog::Print("UBTT_SelectSkill::ExecuteTask Target is nullptr", -1, 10, FColor::Red);
		return EBTNodeResult::Failed;
	}

	int32 lIdx = tIdx;
	int32 rIdx = tIdx;

	if (idx < tIdx)lIdx = tIdx - size;
	else rIdx = tIdx + size;

	if (abs(rIdx - idx) < abs(lIdx - idx))
	{
		aiPawn->SelectRight();
		blackboardComp->SetValueAsObject(SelectedCharacter.SelectedKeyName, targetArray[(idx + 1) % size]);
	}
	else 
	{
		aiPawn->SelectLeft();
		blackboardComp->SetValueAsObject(SelectedCharacter.SelectedKeyName, targetArray[(idx - 1 + size) % size]);
	}

	return EBTNodeResult::Succeeded;
}

FString UBTT_SelectTarget::GetStaticDescription() const
{
	return Super::GetStaticDescription();
}