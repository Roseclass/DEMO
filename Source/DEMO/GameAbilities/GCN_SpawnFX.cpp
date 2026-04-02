#include "GameAbilities/GCN_SpawnFX.h"
#include "Global.h"

#include "Particles/ParticleSystem.h"
#include "Particles/ParticleSystemComponent.h"

#include "NiagaraFunctionLibrary.h"
#include "NiagaraSystem.h"
#include "NiagaraComponent.h"

#include "Characters/TBActiveFXComponent.h"

#include "GameAbilities/GameplayEffectContexts.h"

UGCN_SpawnFX::UGCN_SpawnFX()
{

}

bool UGCN_SpawnFX::OnExecute_Implementation(AActor* MyTarget, const FGameplayCueParameters& Parameters) const
{
	if (!Parameters.OriginalTag.IsValid())
	{
		CLog::Print("UGCN_SpawnFX::OnExecute_Implementation OriginalTag is empty!!!");
		return false;
	}

	const FTurnBasedEffectContext* effectContext = static_cast<const FTurnBasedEffectContext*>(Parameters.EffectContext.Get());
	if (!effectContext)return false;

	UTBActiveFXComponent* fxComp = CHelpers::GetComponent<UTBActiveFXComponent>(MyTarget);
	if (!fxComp)return false;

	TArray<FName> rowNames = SpawnFXRuleDT->GetRowNames();
	for (const FName& rowName : rowNames)
	{
		FGCN_SpawnFXRule* rule = SpawnFXRuleDT->FindRow<FGCN_SpawnFXRule>(rowName, TEXT(""));
		if (!rule)continue;
		if (rule->Tag != Parameters.OriginalTag)continue;
		if (!rule->FXAsset)
		{
			CLog::Print("UGCN_SpawnFX::OnExecute_Implementation FXAsset is empty!!!");
			continue;
		}
		UFXSystemComponent* fx;
		if (rule->SpawnFXType == ESpawnFXType::Duration && fxComp->IsActive(rowName))
		{
			if (Parameters.NormalizedMagnitude < 0)
				fxComp->RemoveFX(rowName);
			else
			{
				fx = fxComp->GetFXFromDataID(rowName);
				for (auto name : rule->Params)
					fx->SetIntParameter(name, Parameters.GameplayEffectLevel);
			}
			continue;
		}

		fx = SpawnFX(rule, effectContext);
		if (rule->SpawnFXType == ESpawnFXType::Duration && fx && !fxComp->IsActive(rowName))
			fxComp->RegisterFX(rowName, fx);
	}

	return false;
}

UFXSystemComponent* UGCN_SpawnFX::SpawnFX(FGCN_SpawnFXRule* Rule, const FTurnBasedEffectContext* EffectContext) const
{
	UParticleSystem* particleFX = Cast<UParticleSystem>(Rule->FXAsset);
	UNiagaraSystem* niagaraFX = Cast<UNiagaraSystem>(Rule->FXAsset);

	UFXSystemComponent* result = nullptr;

	if (Rule->TransformRule == ESpawnFXTransformRule::AttachToActor)
	{
		if (!EffectContext->EffectTargetActor.Get())
		{
			CLog::Print("UGCN_SpawnFX::OnExecute_Implementation AttachToActor failed!!");
			return result;
		}
		USceneComponent* root = EffectContext->EffectTargetActor.Get()->GetRootComponent();
		if (!root)
		{
			CLog::Print("UGCN_SpawnFX::OnExecute_Implementation root is nullptr!!");
			return result;
		}
		if (particleFX)
			result = UGameplayStatics::SpawnEmitterAttached(
				particleFX,
				root,
				FName(),
				Rule->LocationOffset,
				Rule->RotationOffset,
				Rule->Scale
			);
		else if (niagaraFX)
		{
			FFXSystemSpawnParameters params;
			params.bAutoActivate = 1;
			params.bAutoDestroy = 1;
			params.WorldContextObject = GetWorld();
			params.SystemTemplate = niagaraFX;
			params.AttachToComponent = root;
			params.LocationType = EAttachLocation::SnapToTarget;
			params.Location = Rule->LocationOffset;
			params.Rotation = Rule->RotationOffset;
			params.Scale = Rule->Scale;
			result = UNiagaraFunctionLibrary::SpawnSystemAttachedWithParams(params);
		}
		return result;
	}

	if (Rule->TransformRule != ESpawnFXTransformRule::ImpactPoint)
	{
		CLog::Print("UGCN_SpawnFX::OnExecute_Implementation TransformRule error!!");
		return result;
	}

	const FHitResult* hitResult = EffectContext->GetHitResult();
	FVector loc;
	FRotator rot;
	if (hitResult)
	{
		loc = hitResult->ImpactPoint;
		rot = UKismetMathLibrary::MakeRotFromX(hitResult->ImpactNormal);
	}
	else
	{
		FHitResult traceResult;
		TArray<AActor*> ignores;
		ignores.Add(EffectContext->EffectCauserActor.Get());
		UKismetSystemLibrary::LineTraceSingle(
			GetWorld(),
			EffectContext->EffectCauserActor->GetActorLocation(),
			EffectContext->EffectTargetActor->GetActorLocation(),
			ETraceTypeQuery::TraceTypeQuery1,
			1,
			ignores,
			EDrawDebugTrace::None,
			traceResult,
			1);
		loc = traceResult.ImpactPoint;
		rot = UKismetMathLibrary::MakeRotFromX(traceResult.ImpactNormal);
	}


	loc += Rule->LocationOffset;
	rot += Rule->RotationOffset;

	if (particleFX)
		result = UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), particleFX, loc, rot, Rule->Scale, 1);
	else if (niagaraFX)
		result = UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), niagaraFX, loc, rot, Rule->Scale, 1, 1);

	return result;
}
