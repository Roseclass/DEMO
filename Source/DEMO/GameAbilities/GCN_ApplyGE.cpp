#include "GameAbilities/GCN_ApplyGE.h"
#include "Global.h"

#include "GameAbilities/GameplayEffectContexts.h"

#include "TurnBasedSubsystem.h"

UGCN_ApplyGE::UGCN_ApplyGE()
{

}

bool UGCN_ApplyGE::OnExecute_Implementation(AActor* MyTarget, const FGameplayCueParameters& Parameters) const
{
	const FApplyGEContext* effectContext = static_cast<const FApplyGEContext*>(Parameters.EffectContext.Get());
	if (!effectContext)return false;

	UTurnBasedSubsystem* TBS = GetWorld()->GetGameInstance()->GetSubsystem<UTurnBasedSubsystem>();
	TBS->ApplyGE(effectContext);

	return false;
}
