#include "Characters/AI/BTT_SelectSkill.h"
#include "Global.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/BTFunctionLibrary.h"

#include "Characters/TurnBasedCharacter.h"
#include "Characters/AI/TurnBasedEnemy.h"
#include "Characters/AI/TurnBasedAIController.h"
#include "Characters/AI/TurnBasedBehaviorComponent.h"

#include "GameAbilities/AbilityComponent.h"

UBTT_SelectSkill::UBTT_SelectSkill()
{
	NodeName = "SelectSkill";

	SelectedSkillTag.AddStringFilter(this, GET_MEMBER_NAME_CHECKED(UBTT_SelectSkill, SelectedSkillTag));
	TargetSkillTag.AddStringFilter(this, GET_MEMBER_NAME_CHECKED(UBTT_SelectSkill, TargetSkillTag));
}

EBTNodeResult::Type UBTT_SelectSkill::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::ExecuteTask(OwnerComp, NodeMemory);

	UBlackboardComponent* blackboardComp = OwnerComp.GetBlackboardComponent();
	if (!blackboardComp)return EBTNodeResult::Failed;

	AAIController* controller = Cast<AAIController>(OwnerComp.GetOwner());
	UTurnBasedBehaviorComponent* behavior = CHelpers::GetComponent<UTurnBasedBehaviorComponent>(controller);

	ATurnBasedEnemy* aiPawn = Cast<ATurnBasedEnemy>(controller->GetPawn());
	UAbilityComponent* asc = CHelpers::GetComponent<UAbilityComponent>(aiPawn);

	TArray<FGameplayTag> equippedSkillTags = aiPawn->GetEquippedSkillTags();
	int32 size = equippedSkillTags.Num();

	FString currentTag = blackboardComp->GetValueAsString(SelectedSkillTag.SelectedKeyName);
	FString targetTag = blackboardComp->GetValueAsString(TargetSkillTag.SelectedKeyName);

	if (currentTag == targetTag) return EBTNodeResult::Succeeded;

	int32 idx = -1;
	int32 tIdx = -1;

	for (int32 i = 0; i < size; i++)
	{
		if (equippedSkillTags[i].ToString() == currentTag)idx = i;
		if (equippedSkillTags[i].ToString() == targetTag)tIdx = i;
	}

	if (idx < 0)
	{
		CLog::Print("UBTT_SelectSkill::ExecuteTask currentTag is NOT VALID", -1, 10, FColor::Red);
		return EBTNodeResult::Failed;
	}

	if (tIdx < 0)
	{
		CLog::Print("UBTT_SelectSkill::ExecuteTask targetTag is NOT VALID", -1, 10, FColor::Red);
		return EBTNodeResult::Failed;
	}

	int32 uIdx = tIdx;
	int32 dIdx = tIdx;

	if (idx < tIdx)uIdx = tIdx - size;
	else dIdx = tIdx + size;

	if (abs(dIdx - idx) < abs(uIdx - idx))
	{
		aiPawn->SelectDown();
		blackboardComp->SetValueAsString(SelectedSkillTag.SelectedKeyName, equippedSkillTags[(idx + 1) % size].ToString());
	}
	else
	{
		aiPawn->SelectUp();
		blackboardComp->SetValueAsString(SelectedSkillTag.SelectedKeyName, equippedSkillTags[(idx - 1 + size) % size].ToString());
	}

	return EBTNodeResult::Succeeded;
}

FString UBTT_SelectSkill::GetStaticDescription() const
{
	return Super::GetStaticDescription();
}