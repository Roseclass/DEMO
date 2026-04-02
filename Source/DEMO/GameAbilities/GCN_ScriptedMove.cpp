#include "GameAbilities/GCN_ScriptedMove.h"
#include "Global.h"

#include "Characters/TurnBasedCharacter.h"

#include "GameAbilities/GameplayEffectContexts.h"

#include "TurnBasedSubsystem.h"

UGCN_ScriptedMove::UGCN_ScriptedMove()
{

}

bool UGCN_ScriptedMove::OnExecute_Implementation(AActor* MyTarget, const FGameplayCueParameters& Parameters) const
{
	const FScriptedMoveContext* effectContext = static_cast<const FScriptedMoveContext*>(Parameters.EffectContext.Get());
	if (!effectContext)return false;

	UTurnBasedSubsystem* TBS = GetWorld()->GetGameInstance()->GetSubsystem<UTurnBasedSubsystem>();
	TBS->EnqueueScriptedMove(effectContext);

	return false;
}
