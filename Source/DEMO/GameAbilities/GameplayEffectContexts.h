#pragma once

#include "CoreMinimal.h"
#include "UObject/ObjectMacros.h"
#include "UObject/Object.h"
#include "UObject/Class.h"
#include "GameplayEffectTypes.h"
#include "GameplayEffectContexts.generated.h"

/**
 * 
 */

//#include "Abilities/GameplayEffectContexts.h"

USTRUCT(BlueprintType)
struct FDamageEffectContext : public FGameplayEffectContext
{
    GENERATED_BODY()

public:
    UPROPERTY()float BaseDamage;
    UPROPERTY()float CalculatedDamage;
    UPROPERTY()bool bIsCritical;
};

//class AWarningSign;

USTRUCT(BlueprintType)
struct FWarningSignEffectContext : public FGameplayEffectContext
{
    GENERATED_BODY()
public:
    //UPROPERTY()TSubclassOf<AWarningSign> Class;
    UPROPERTY()FTransform Transform;
    UPROPERTY()AActor* Owner;
    UPROPERTY()ESpawnActorCollisionHandlingMethod CollisionHandlingOverride;
    UPROPERTY()float StartServerTime;
    UPROPERTY()float Duration;
    UPROPERTY()float ExtraDuration;
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
