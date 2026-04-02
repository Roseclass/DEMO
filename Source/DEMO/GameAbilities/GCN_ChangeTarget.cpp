#include "GameAbilities/GCN_ChangeTarget.h"
#include "Global.h"

#include "Characters/TurnBasedCharacter.h"

#include "GameAbilities/GameplayEffectContexts.h"

#include "TurnBasedSubsystem.h"

UGCN_ChangeTarget::UGCN_ChangeTarget()
{

}

bool UGCN_ChangeTarget::OnExecute_Implementation(AActor* MyTarget, const FGameplayCueParameters& Parameters) const
{
	const FChangeTargetContext* effectContext = static_cast<const FChangeTargetContext*>(Parameters.EffectContext.Get());
	if (!effectContext)return false;

	ATurnBasedCharacter* target = Cast<ATurnBasedCharacter>(effectContext->EventCauserActor);
	if (!target)return false;

	TArray<ATurnBasedCharacter*> PreTargets;

	if (effectContext->Data.PreTarget == EPayloadActorType::EventCauser)
		PreTargets.Add(Cast<ATurnBasedCharacter>(effectContext->EventCauserActor));
	else if (effectContext->Data.PreTarget == EPayloadActorType::EventTargets)
		for(auto ch : effectContext->EventTargetActors)
			PreTargets.Add(Cast<ATurnBasedCharacter>(ch));
	else if (effectContext->Data.PreTarget == EPayloadActorType::RuleSource)
		PreTargets.Add(Cast<ATurnBasedCharacter>(effectContext->RuleSourceActor));

	UTurnBasedSubsystem* TBS = GetWorld()->GetGameInstance()->GetSubsystem<UTurnBasedSubsystem>();
	TBS->ChangeTarget(PreTargets, target);

	return false;
}
