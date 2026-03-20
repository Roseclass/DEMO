#include "GameAbilities/GCN_MoveCamera.h"
#include "Global.h"

#include "GameAbilities/GameplayEffectPayloads.h"

#include "TurnBasedSubsystem.h"

UGCN_MoveCamera::UGCN_MoveCamera()
{

}

bool UGCN_MoveCamera::OnExecute_Implementation(AActor* MyTarget, const FGameplayCueParameters& Parameters) const
{
	const FPayloadContext* effectContext = static_cast<const FPayloadContext*>(Parameters.EffectContext.Get());
	if (!effectContext)return false;

	UTurnBasedSubsystem* TBS = GetWorld()->GetGameInstance()->GetSubsystem<UTurnBasedSubsystem>();
	TBS->ApplyCameraMove(effectContext);

	return false;
}
