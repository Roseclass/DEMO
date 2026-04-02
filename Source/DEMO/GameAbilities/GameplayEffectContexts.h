#pragma once

#include "CoreMinimal.h"
#include "UObject/ObjectMacros.h"
#include "UObject/Object.h"
#include "UObject/Class.h"
#include "GameplayEffectTypes.h"
#include "GameplayTagContainer.h"
#include "Engine/DataAsset.h"
#include "GameAbilities/GameplayEffectPayloads.h"
#include "GameplayEffectContexts.generated.h"

/**
 * 
 */

//#include "GameAbilities/GameplayEffectContexts.h"


USTRUCT(BlueprintType)
struct FTurnBasedEffectContext : public FGameplayEffectContext
{
    GENERATED_BODY()
public:
    virtual FTurnBasedEffectContext* Duplicate() const override
    {
        FTurnBasedEffectContext* NewContext = new FTurnBasedEffectContext();
        *NewContext = *this;
        if (GetHitResult())
        {
            // Does a deep copy of the hit result
            NewContext->AddHitResult(*GetHitResult(), true);
        }
        return NewContext;
    }
public:
    TWeakObjectPtr<AActor> EffectSourceActor;            // 이펙트 출처 액터
    TWeakObjectPtr<AActor> EffectCauserActor;            // 이펙트 생성 액터
    TWeakObjectPtr<AActor> EffectTargetActor;            // 이펙트 적용 액터
    TWeakObjectPtr<AActor> SkillCauserActor;             // 스킬 시전 액터
    TArray<TWeakObjectPtr<AActor>> SkillTargetActors;    // 스킬 대상 액터

    TWeakObjectPtr<AActor> RuleSourceActor;             // 규칙/상태를 제공한 주체
    TWeakObjectPtr<AActor> EventCauserActor;            // 트리거 사건 유발자
    TArray<TWeakObjectPtr<AActor>> EventTargetActors;   // 현재 이벤트 대상
};

USTRUCT()
struct FApplyGEContext : public FTurnBasedEffectContext
{
    GENERATED_BODY()
public:
    virtual FApplyGEContext* Duplicate() const override
    {
        FApplyGEContext* NewContext = new FApplyGEContext();
        *NewContext = *this;
        if (GetHitResult())
        {
            // Does a deep copy of the hit result
            NewContext->AddHitResult(*GetHitResult(), true);
        }
        return NewContext;
    }
public:
    FApplyGEData Data;
};

USTRUCT()
struct FMoveCameraContext : public FTurnBasedEffectContext
{
    GENERATED_BODY()
public:
    virtual FMoveCameraContext* Duplicate() const override
    {
        FMoveCameraContext* NewContext = new FMoveCameraContext();
        *NewContext = *this;
        if (GetHitResult())
        {
            // Does a deep copy of the hit result
            NewContext->AddHitResult(*GetHitResult(), true);
        }
        return NewContext;
    }
public:
    FMoveCameraData Data;
};

USTRUCT()
struct FReserveActionContext : public FTurnBasedEffectContext
{
    GENERATED_BODY()
public:
    virtual FReserveActionContext* Duplicate() const override
    {
        FReserveActionContext* NewContext = new FReserveActionContext();
        *NewContext = *this;
        if (GetHitResult())
        {
            // Does a deep copy of the hit result
            NewContext->AddHitResult(*GetHitResult(), true);
        }
        return NewContext;
    }
public:
    FReserveActionData Data;
};

USTRUCT()
struct FSpawnDamageDealerContext : public FTurnBasedEffectContext
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
    FSpawnDamageDealerData Data;
};

USTRUCT()
struct FChangeTargetContext : public FTurnBasedEffectContext
{
    GENERATED_BODY()
public:
    virtual FChangeTargetContext* Duplicate() const override
    {
        FChangeTargetContext* NewContext = new FChangeTargetContext();
        *NewContext = *this;
        if (GetHitResult())
        {
            // Does a deep copy of the hit result
            NewContext->AddHitResult(*GetHitResult(), true);
        }
        return NewContext;
    }
public:
    FChangeTargetData Data;
};

USTRUCT()
struct FScriptedMoveContext : public FTurnBasedEffectContext
{
    GENERATED_BODY()
public:
    virtual FScriptedMoveContext* Duplicate() const override
    {
        FScriptedMoveContext* NewContext = new FScriptedMoveContext();
        *NewContext = *this;
        if (GetHitResult())
        {
            // Does a deep copy of the hit result
            NewContext->AddHitResult(*GetHitResult(), true);
        }
        return NewContext;
    }
public:
    TWeakObjectPtr<AActor> MoveInstigator;
    FScriptedMoveData Data;
};

USTRUCT(BlueprintType)
struct FDamageEffectContext : public FGameplayEffectContext
{
    GENERATED_BODY()

public:
    UPROPERTY()FVector Location;
    UPROPERTY()float BaseDamage;
    UPROPERTY()float CalculatedDamage;
    UPROPERTY()bool bIsCritical;
};

USTRUCT(BlueprintType)
struct FEffectEventContext : public FGameplayEffectContext
{
    GENERATED_BODY()
public:
    virtual FEffectEventContext* Duplicate() const override
    {
        FEffectEventContext* NewContext = new FEffectEventContext();
        *NewContext = *this;
        if (GetHitResult())
        {
            // Does a deep copy of the hit result
            NewContext->AddHitResult(*GetHitResult(), true);
        }
        return NewContext;
    }
public:
    TWeakObjectPtr<AActor> EventCauserActor;            // 트리거 사건 유발자
    TWeakObjectPtr<AActor> EventTargetActor;            // 현재 이벤트 대상
};
