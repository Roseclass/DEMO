#include "GameAbilities/GCN_DamageText.h"
#include "Global.h"

#include "GameAbilities/GameplayEffectContexts.h"
#include "Objects/DamageText.h"

UGCN_DamageText::UGCN_DamageText()
{

}

bool UGCN_DamageText::OnExecute_Implementation(AActor* MyTarget, const FGameplayCueParameters& Parameters) const
{
	const FDamageEffectContext* effectContext = static_cast<const FDamageEffectContext*>(Parameters.EffectContext.Get());
	if (!effectContext)return false;

	FTransform transform;
	transform.SetTranslation(effectContext->GetOrigin());
	ADamageText* txt = GetWorld()->SpawnActorDeferred<ADamageText>(DamageTextClass, transform);
	txt->Init(effectContext->CalculatedDamage, effectContext->bIsCritical);
	UGameplayStatics::FinishSpawningActor(txt, transform);

	return false;
}