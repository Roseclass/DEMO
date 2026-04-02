#include "GameAbilities/GCN_ReserveAction.h"
#include "Global.h"

#include "GameAbilities/GameplayEffectContexts.h"

#include "TurnBasedSubsystem.h"

UGCN_ReserveAction::UGCN_ReserveAction()
{

}

bool UGCN_ReserveAction::OnExecute_Implementation(AActor* MyTarget, const FGameplayCueParameters& Parameters) const
{
	// TODO:: 여기서 예약하지 말고 옮기기

	const FReserveActionContext* effectContext = static_cast<const FReserveActionContext*>(Parameters.EffectContext.Get());
	if (!effectContext)return false;

	UTurnBasedSubsystem* TBS = GetWorld()->GetGameInstance()->GetSubsystem<UTurnBasedSubsystem>();
	TBS->ReserveAction(effectContext);

	return false;
}
