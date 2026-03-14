#pragma once

#include "CoreMinimal.h"
#include "UObject/ObjectMacros.h"
#include "UObject/Object.h"
#include "UObject/Class.h"
#include "GameplayEffectTypes.h"
#include "GameplayTagContainer.h"
#include "Engine/DataTable.h"
#include "ExecutionContextTypes.generated.h"

/**
 * 
 */

//#include "GameAbilities/ExecutionContextTypes.h"

UENUM(BlueprintType)
enum class EExecutionResolveType : uint8
{
    Damage, Heal,
    MAX UMETA(Hidden)
};

UENUM(BlueprintType)
enum class EExecutionReferenceActorType : uint8
{
    EffectSource, EffectTarget, EffectCauser, SkillTarget, SkillCauser, 
    MAX UMETA(Hidden)
};

UENUM(BlueprintType)    
enum class EExecutionMeasureType : uint8
{
    Always, HasTag, HasTagExact, Attribute,
    MAX UMETA(Hidden)
};

UENUM(BlueprintType)
enum class EExecutionAttributeType : uint8
{
    Health, HealthRatio, Power, Speed,
    MAX UMETA(Hidden)
};

UENUM(BlueprintType)
enum class EExecutionCompareType : uint8
{
    Less, LessOrEqual, Equal, GreaterOrEqual, Greater,
    MAX UMETA(Hidden)
};

UENUM(BlueprintType)    
enum class EExecutionModifyOp : uint8
{
    // Add Value
    AddConstant, 
    // Add Count * Value
    AddMeasuredValueScaled, 
    // Multiply Value
    MultiplyConstant, 
    // Multiply (Count * Value)
    MultiplyFromMeasuredValue, 
    MAX UMETA(Hidden)
};

USTRUCT(BlueprintType)
struct FExecutionContext : public FGameplayEffectContext
{
    GENERATED_BODY()
public:
    virtual FExecutionContext* Duplicate() const override
    {
        FExecutionContext* NewContext = new FExecutionContext();
        *NewContext = *this;
        if (GetHitResult())
        {
            // Does a deep copy of the hit result
            NewContext->AddHitResult(*GetHitResult(), true);
        }
        return NewContext;
    }
public:
    TWeakObjectPtr<AActor> EffectSourceActor;   // 이펙트 출처 액터
    TWeakObjectPtr<AActor> EffectCauserActor;   // 이펙트 생성 액터
    TWeakObjectPtr<AActor> EffectTargetActor;   // 이펙트 적용 액터
    TWeakObjectPtr<AActor> SkillCauserActor;    // 스킬 시전 액터
    TWeakObjectPtr<AActor> SkillTargetActor;    // 스킬 대상 액터
};

USTRUCT(BlueprintType)
struct FExecutionModifyData
{
    GENERATED_BODY()
public:
    UPROPERTY(EditDefaultsOnly)
        EExecutionReferenceActorType ReferenceActor;

    UPROPERTY(EditDefaultsOnly)
        EExecutionMeasureType MeasureType;

    UPROPERTY(EditDefaultsOnly, meta = (EditCondition = "MeasureType == EExecutionMeasureType::HasTag || MeasureType == EExecutionMeasureType::HasTagExact", EditConditionHides))
        FGameplayTag Tag;

    UPROPERTY(EditDefaultsOnly, meta = (EditCondition = "MeasureType == EExecutionMeasureType::Attribute", EditConditionHides))
        EExecutionAttributeType AttributeType;

    UPROPERTY(EditDefaultsOnly, meta = (EditCondition = "MeasureType != EExecutionMeasureType::Always && MeasureType != EExecutionMeasureType::HasTagExact", EditConditionHides))
        EExecutionCompareType CompareType = EExecutionCompareType::Equal;

    UPROPERTY(EditDefaultsOnly, meta = (EditCondition = "MeasureType != EExecutionMeasureType::Always && MeasureType != EExecutionMeasureType::HasTagExact", EditConditionHides))
        float Threshold = 0.0f;

    UPROPERTY(EditDefaultsOnly)
        float Value;

    UPROPERTY(EditDefaultsOnly)
        EExecutionModifyOp ModifyOp;

};

USTRUCT(BlueprintType)
struct FExecutionModifyRule : public FTableRowBase
{
    GENERATED_BODY()
public:
    UPROPERTY(EditDefaultsOnly)
        EExecutionResolveType ResolveType;

    UPROPERTY(EditDefaultsOnly)
        FExecutionModifyData ResolveData;
};