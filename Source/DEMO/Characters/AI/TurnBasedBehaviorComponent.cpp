#include "Characters/AI/TurnBasedBehaviorComponent.h"
#include "Global.h"
#include "BehaviorTree/BlackboardComponent.h"

UTurnBasedBehaviorComponent::UTurnBasedBehaviorComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UTurnBasedBehaviorComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UTurnBasedBehaviorComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if(IsWaitMode())
	{
		CLog::Print("Wait", -1, 0, FColor::Turquoise);
	}
	else if (IsSelectSkillMode())
	{
		CLog::Print("Skill", -1, 0, FColor::Turquoise);
	}
	else if(IsSelectTargetMode())
	{
		CLog::Print("Target", -1, 0, FColor::Turquoise);
	}
	else if(IsSkillSequenceMode())
	{
		CLog::Print("Sequence", -1, 0, FColor::Turquoise);
	}
}

void UTurnBasedBehaviorComponent::ChangeType(EBehaviorType InNewType)
{
	Blackboard->SetValueAsEnum(BehaviorTypeKeyName, uint8(InNewType));
}

void UTurnBasedBehaviorComponent::SetNotMyTurnMode()
{
	ChangeType(EBehaviorType::NotMyTurn);
}

void UTurnBasedBehaviorComponent::SetWaitMode()
{
	ChangeType(EBehaviorType::Wait);
}

void UTurnBasedBehaviorComponent::SetSelectSkillMode()
{
	ChangeType(EBehaviorType::SelectSkill);
}

void UTurnBasedBehaviorComponent::SetSelectTargetMode()
{
	ChangeType(EBehaviorType::SelectTarget);
}

void UTurnBasedBehaviorComponent::SetSkillSequenceMode()
{
	ChangeType(EBehaviorType::SkillSequence);
}

bool UTurnBasedBehaviorComponent::IsNotMyTurnMode()
{
	return Blackboard->GetValueAsEnum(BehaviorTypeKeyName) == uint8(EBehaviorType::NotMyTurn);
}

bool UTurnBasedBehaviorComponent::IsWaitMode()
{
	return Blackboard->GetValueAsEnum(BehaviorTypeKeyName) == uint8(EBehaviorType::Wait);
}

bool UTurnBasedBehaviorComponent::IsSelectSkillMode()
{
	return Blackboard->GetValueAsEnum(BehaviorTypeKeyName) == uint8(EBehaviorType::SelectSkill);
}

bool UTurnBasedBehaviorComponent::IsSelectTargetMode()
{
	return Blackboard->GetValueAsEnum(BehaviorTypeKeyName) == uint8(EBehaviorType::SelectTarget);
}

bool UTurnBasedBehaviorComponent::IsSkillSequenceMode()
{
	return Blackboard->GetValueAsEnum(BehaviorTypeKeyName) == uint8(EBehaviorType::SkillSequence);
}