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
	// TODO:: 여기서 예약하지 말고 옮기기

	const FPayloadContext* effectContext = static_cast<const FPayloadContext*>(Parameters.EffectContext.Get());
	if (!effectContext)return false;

	ATurnBasedCharacter* target = Cast<ATurnBasedCharacter>(effectContext->EventCauserActor);
	if (!target)return false;

	UTurnBasedSubsystem* TBS = GetWorld()->GetGameInstance()->GetSubsystem<UTurnBasedSubsystem>();
	TBS->ChangeTarget(target);

	return false;
}
