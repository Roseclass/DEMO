#include "GameAbilities/GCN_MoveCamera.h"
#include "Global.h"
#include "Objects/TurnBasedPhaseManager.h"

#include "GameAbilities/GameplayEffectContexts.h"

#include "Objects/TurnbasedPhaseCamera.h"

UGCN_MoveCamera::UGCN_MoveCamera()
{

}

bool UGCN_MoveCamera::OnExecute_Implementation(AActor* MyTarget, const FGameplayCueParameters& Parameters) const
{
	const FCameraMoveEffectContext* effectContext = static_cast<const FCameraMoveEffectContext*>(Parameters.EffectContext.Get());
	if (!effectContext)return false;

	ATurnbasedPhaseCamera* camera = Cast<ATurnbasedPhaseCamera>(UGameplayStatics::GetActorOfClass(GetWorld(), CameraActorClass));
	if (!camera)return false;
	
	camera->ApplyCameraMove(effectContext);

	return false;
}
