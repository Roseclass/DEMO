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

	ATurnBasedCharacter* instigator = Cast<ATurnBasedCharacter>(effectContext->EventCauserActor.Get());
	if (!instigator)
	{
		CLog::Print("UGCN_SpawnDamageDealer::OnExecute_Implementation instigator cast failed!!");
		return false;
	}

	for (auto target : effectContext->EventTargetActors)
	{
		FSpawnDamageDealerData data = effectContext->Data;
		data.TargetActor = target;
		float delay = UKismetMathLibrary::RandomFloatInRange(effectContext->Data.DelayMin, effectContext->Data.DelayMax);
		FTimerDelegate func =
			FTimerDelegate::CreateLambda([data, instigator]()
				{

					FTransform transform;
					if (data.bUseSocketLocation)
					{
						transform.SetTranslation(instigator->GetMesh()->GetSocketLocation(data.SocketName));

						if (data.bIsHoming)
						{
							FVector baseDir = (instigator->GetMesh()->GetSocketRotation(data.SocketName) + data.SpawnDirectionOffset).Vector();
							FVector randomDir = UKismetMathLibrary::RandomUnitVectorInConeInDegrees(baseDir, data.SpawnAngle);
							transform.SetRotation(randomDir.ToOrientationQuat()); 
						}
					}
					else
					{
						FVector loc = instigator->GetActorLocation();
						loc += instigator->GetActorForwardVector() * data.FrontDist;
						loc += instigator->GetActorUpVector() * data.UpDist;
						loc += instigator->GetActorRightVector() * data.RightDist;
						transform.SetTranslation(loc);

						if (data.bIsHoming)
						{
							FVector baseDir = (instigator->GetActorRotation() + data.SpawnDirectionOffset).Vector();
							FVector randomDir = UKismetMathLibrary::RandomUnitVectorInConeInDegrees(baseDir, data.SpawnAngle);
							transform.SetRotation(randomDir.ToOrientationQuat());
						}
					}

					if(!data.bIsHoming)
						transform.SetRotation(FQuat(UKismetMathLibrary::FindLookAtRotation(transform.GetTranslation(), data.TargetActor.Get()->GetActorLocation())));

					ADamageDealer* dealer = instigator->GetWorld()->SpawnActorDeferred<ADamageDealer>(
						data.Class,
						transform,
						instigator,
						instigator,
						ESpawnActorCollisionHandlingMethod::AlwaysSpawn);

					dealer->Init(&data);

					UGameplayStatics::FinishSpawningActor(dealer, transform);
				});
		if (delay <= 1e-9)func.Execute();
		else
		{
			FTimerHandle handle;
			GetWorld()->GetTimerManager().SetTimer(handle, func, delay, false);
		}
	}

	return false;
}
