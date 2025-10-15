#include "GameAbilities/GCN_SpawnDamageDealer.h"
#include "Global.h"

#include "Characters/TurnBasedCharacter.h"

#include "GameAbilities/GameplayEffectContexts.h"

#include "Objects/DamageDealer.h"

UGCN_SpawnDamageDealer::UGCN_SpawnDamageDealer()
{

}

bool UGCN_SpawnDamageDealer::OnExecute_Implementation(AActor* MyTarget, const FGameplayCueParameters& Parameters) const
{
	const FSpawnDamageDealerContext* effectContext = static_cast<const FSpawnDamageDealerContext*>(Parameters.EffectContext.Get());
	if (!effectContext)
	{
		CLog::Print("UGCN_SpawnDamageDealer::OnExecute_Implementation context cast failed !!");
		return false;
	}

	ATurnBasedCharacter* instigator = Cast<ATurnBasedCharacter>(effectContext->GetInstigator());
	if (!instigator)
	{
		CLog::Print("UGCN_SpawnDamageDealer::OnExecute_Implementation instigator cast failed!!");
		return false;
	}

	FTransform transform;
	if (effectContext->bUseSocketLocation)
		transform.SetTranslation(instigator->GetMesh()->GetSocketLocation(effectContext->SocketName));
	else
	{
		FVector loc = instigator->GetActorLocation();
		loc += instigator->GetActorForwardVector() * effectContext->FrontDist;
		loc += instigator->GetActorUpVector() * effectContext->UpDist;
		loc += instigator->GetActorRightVector() * effectContext->RightDist;
		transform.SetTranslation(loc);
	}

	transform.SetRotation(FQuat(UKismetMathLibrary::FindLookAtRotation(transform.GetTranslation(), effectContext->TargetActor.Get()->GetActorLocation())));

	ADamageDealer* dealer = GetWorld()->SpawnActorDeferred<ADamageDealer>(
		effectContext->Class, 
		transform, 
		instigator,
		instigator,
		ESpawnActorCollisionHandlingMethod::AlwaysSpawn);

	dealer->Init(effectContext);

	UGameplayStatics::FinishSpawningActor(dealer, transform);

	return false;
}
