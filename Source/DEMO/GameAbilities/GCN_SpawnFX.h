#pragma once

#include "CoreMinimal.h"
#include "GameplayCueNotify_Static.h"
#include "GameplayTagContainer.h"
#include "Engine/DataTable.h"
#include "GCN_SpawnFX.generated.h"

/**
 * 
 */

class UFXSystemAsset;
class UFXSystemComponent;
struct FTurnBasedEffectContext;

UENUM(BlueprintType)
enum class ESpawnFXTransformRule : uint8
{
	AttachToActor, ImpactPoint,
	MAX UMETA(Hidden)
};

UENUM(BlueprintType)
enum class ESpawnFXType : uint8
{
	Burst, Duration,
	MAX UMETA(Hidden)
};

USTRUCT(BlueprintType)
struct FGCN_SpawnFXRule : public FTableRowBase
{
    GENERATED_BODY()
public:
	UPROPERTY(EditDefaultsOnly)
		FGameplayTag Tag;

	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = "true", DisplayThumbnail = "true", DisplayName = "Particle", AllowedClasses = "ParticleSystem, NiagaraSystem"))
		UFXSystemAsset* FXAsset;

	UPROPERTY(EditDefaultsOnly)
		ESpawnFXType SpawnFXType;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Condition", meta = (EditCondition = "SpawnFXType == ESpawnFXType::Duration", EditConditionHides))
		TArray<FName> Params;

	UPROPERTY(EditDefaultsOnly)
		ESpawnFXTransformRule TransformRule;

	UPROPERTY(EditDefaultsOnly)
		FVector LocationOffset;

	UPROPERTY(EditDefaultsOnly)
		FRotator RotationOffset;

	UPROPERTY(EditDefaultsOnly)
		FVector Scale = FVector(1);
};

UCLASS()
class DEMO_API UGCN_SpawnFX : public UGameplayCueNotify_Static
{
	GENERATED_BODY()
public:
	UGCN_SpawnFX();
protected:
public:
	virtual bool OnExecute_Implementation(AActor* MyTarget, const FGameplayCueParameters& Parameters) const override;

	//property
private:
protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		TObjectPtr<UDataTable> SpawnFXRuleDT;

public:

	//function
private:
	UFXSystemComponent* SpawnFX(FGCN_SpawnFXRule* Rule, const FTurnBasedEffectContext* EffectContext) const;
protected:
public:
};
