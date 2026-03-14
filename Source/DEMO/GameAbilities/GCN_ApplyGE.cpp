#include "GameAbilities/GCN_ApplyGE.h"
#include "Global.h"

#include "GameAbilities/GameplayEffectContexts.h"

#include "TurnBasedSubsystem.h"

UGCN_ApplyGE::UGCN_ApplyGE()
{

}

bool UGCN_ApplyGE::OnExecute_Implementation(AActor* MyTarget, const FGameplayCueParameters& Parameters) const
{
	// TODO:: 여기서 예약하지 말고 옮기기

	const FPayloadContext* effectContext = static_cast<const FPayloadContext*>(Parameters.EffectContext.Get());
	if (!effectContext)return false;

	UTurnBasedSubsystem* TBS = GetWorld()->GetGameInstance()->GetSubsystem<UTurnBasedSubsystem>();
	TBS->ApplyGE(effectContext);

	return false;
}
