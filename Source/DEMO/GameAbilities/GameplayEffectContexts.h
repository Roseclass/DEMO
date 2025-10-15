#pragma once

#include "CoreMinimal.h"
#include "UObject/ObjectMacros.h"
#include "UObject/Object.h"
#include "UObject/Class.h"
#include "GameplayEffectTypes.h"
#include "GameplayTagContainer.h"
#include "GameplayEffectContexts.generated.h"

/**
 * 
 */

//#include "GameAbilities/GameplayEffectContexts.h"

UENUM(BlueprintType)
enum class ECameraReturnType : uint8
{
    NONE,
    Prev,
    Initial,
    MAX UMETA(Hidden)
};

UENUM(BlueprintType)
enum class ECameraGoalActorType : uint8
{
    Owner,
    Target,
    Projectile,
    MAX UMETA(Hidden)
};

UENUM(BlueprintType)
enum class ECameraMoveType : uint8
{
    SplineLocation,
    FixedLocation,
    CurrentToGoal,
    StartToGoal,
    Target,
    MAX UMETA(Hidden)
};

UENUM(BlueprintType)
enum class ECameraLookAtType : uint8
{
    SplineLocation,
    FixedLocation, 
    CurrentToGoal,
    StartToGoal,
    Target,
    MAX UMETA(Hidden)
};

USTRUCT(BlueprintType)
struct FCameraMoveEffectContext : public FGameplayEffectContext
{
    GENERATED_BODY()
public:
    virtual FGameplayEffectContext* Duplicate() const override
    {
        FCameraMoveEffectContext* NewContext = new FCameraMoveEffectContext();
        *NewContext = *this;
        if (GetHitResult())
        {
            // Does a deep copy of the hit result
            NewContext->AddHitResult(*GetHitResult(), true);
        }
        return NewContext;
    }

public:
    //Default
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Data|Default", meta = (ToolTip = "if this<0, there's no next event"))
        float NextEventDelay = -1;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Data|Default")
        float Duration = 0.1;

    UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Data|Default")
        bool bAutoReturn = 0;

    UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Data|Default", meta = (EditCondition = "bAutoReturn", EditConditionHides))
        ECameraReturnType CameraReturnType;

    UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Data|Default")
        float ReturnBlendTime = 0.1;

    UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Data|Default")
        float ArmLength;

    UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Data|Default")
        bool bEnableArmCollision;

    //Location
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Data|Location")
        ECameraMoveType CameraMoveType;

    UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Data|Location", meta = (DisplayName = "TargetActorType", EditCondition = "CameraMoveType == ECameraMoveType::Target", EditConditionHides))
        ECameraGoalActorType LocationTargetActorType = ECameraGoalActorType::Owner;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Data|Location", meta = (DisplayName = "Location", EditCondition = "CameraMoveType == ECameraMoveType::StartToGoal", EditConditionHides))
        FVector StartLocation;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Data|Location", meta = (DisplayName = "Location", EditCondition = "CameraMoveType == ECameraMoveType::CurrentToGoal || CameraMoveType == ECameraMoveType::StartToGoal", EditConditionHides))
        FVector Location;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Data|Location", meta = (DisplayName = "Offset"))
        FVector LocationOffset;

    //LookAt
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Data|LookAt")
        ECameraLookAtType CameraLookAtType;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Data|LookAt", meta = (DisplayName = "TargetActorType", EditCondition = "CameraLookAtType == ECameraLookAtType::Target", EditConditionHides))
        ECameraGoalActorType LookAtTargetActorType = ECameraGoalActorType::Owner;

    UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Data|LookAt", meta = (DisplayName = "Location", EditCondition = "CameraLookAtType == ECameraLookAtType::StartToGoal", EditConditionHides))
        FVector StartLookAtLocation;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Data|LookAt", meta = (DisplayName = "Location", EditCondition = "CameraLookAtType == ECameraLookAtType::CurrentToGoal && CameraLookAtType == ECameraLookAtType::StartToGoal", EditConditionHides))
        FVector LookAtLocation;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Data|LookAt", meta = (DisplayName = "Offset"))
        FVector LookAtOffset;
};

class ADamageDealer;

USTRUCT(BlueprintType)
struct FDamageDealerTriggerData
{
    GENERATED_BODY()
public:
    UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
        float Delay;

    UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
        FGameplayTag Tag;
};

USTRUCT(BlueprintType)
struct FSpawnDamageDealerContext : public FGameplayEffectContext
{
    GENERATED_BODY()
public:
    virtual FSpawnDamageDealerContext* Duplicate() const override
    {
        FSpawnDamageDealerContext* NewContext = new FSpawnDamageDealerContext();
        *NewContext = *this;
        if (GetHitResult())
        {
            // Does a deep copy of the hit result
            NewContext->AddHitResult(*GetHitResult(), true);
        }
        return NewContext;
    }
public:
    TWeakObjectPtr<AActor> TargetActor;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
        TSubclassOf<ADamageDealer> Class;

    UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
        FName SocketName;

    UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
        TArray<FDamageDealerTriggerData> ActivateTriggerDatas;

    UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
        TArray<FDamageDealerTriggerData> CollisionTriggerDatas;

    UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
        TArray<FDamageDealerTriggerData> DamageSendTriggerDatas;

    UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
        TArray<FDamageDealerTriggerData> DeactivateTriggerDatas;

    UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
        bool bUseSocketLocation = 1;

    UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, meta = (EditCondition = "!bUseSocketLocation", EditConditionHides))
        float FrontDist;

    UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, meta = (EditCondition = "!bUseSocketLocation", EditConditionHides))
        float RightDist;

    UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, meta = (EditCondition = "!bUseSocketLocation", EditConditionHides))
        float UpDist;
};

USTRUCT(BlueprintType)
struct FDamageEffectContext : public FGameplayEffectContext
{
    GENERATED_BODY()

public:
    UPROPERTY()float BaseDamage;
    UPROPERTY()float CalculatedDamage;
    UPROPERTY()bool bIsCritical;
};

class UNiagaraSystem;

USTRUCT(BlueprintType)
struct FNiagaraFXEffectContext : public FGameplayEffectContext
{
    GENERATED_BODY()
public:
    UPROPERTY()UNiagaraSystem* FX;
    UPROPERTY()FTransform Transform;
    UPROPERTY()AActor* AttachTarget;
};
