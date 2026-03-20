#pragma once

#include "CoreMinimal.h"
#include "UObject/ObjectMacros.h"
#include "UObject/Object.h"
#include "UObject/Class.h"
#include "GameplayEffectTypes.h"
#include "GameplayTagContainer.h"
#include "Engine/DataAsset.h"
#include "GameplayEffectContexts.generated.h"

/**
 * 
 */

//#include "GameAbilities/GameplayEffectContexts.h"

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
    UPROPERTY()FVector Location;
    UPROPERTY()float BaseDamage;
    UPROPERTY()float CalculatedDamage;
    UPROPERTY()bool bIsCritical;
};

USTRUCT(BlueprintType)
struct FPayloadContext : public FGameplayEffectContext
{
    GENERATED_BODY()
public:
    virtual FPayloadContext* Duplicate() const override
    {
        FPayloadContext* NewContext = new FPayloadContext();
        *NewContext = *this;
        if (GetHitResult())
        {
            // Does a deep copy of the hit result
            NewContext->AddHitResult(*GetHitResult(), true);
        }
        return NewContext;
    }
public:
    UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
        FGameplayTag GCNTag;

    UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
        TObjectPtr<UPrimaryDataAsset> Payload;

    TWeakObjectPtr<AActor> RuleSourceActor;     // 규칙/상태를 제공한 주체
    TWeakObjectPtr<AActor> EventCauserActor;    // 트리거 사건 유발자
    TWeakObjectPtr<AActor> EventTargetActor;    // 현재 이벤트 대상
    TArray<TWeakObjectPtr<AActor>> EventTargetActors;    // 현재 이벤트 대상이 여럿이라면
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
    TWeakObjectPtr<AActor> EventCauserActor;    // 트리거 사건 유발자
    TWeakObjectPtr<AActor> EventTargetActor;    // 현재 이벤트 대상
};
