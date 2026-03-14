#pragma once

#include "CoreMinimal.h"
#include "UObject/ObjectMacros.h"
#include "UObject/Object.h"
#include "GameplayEffectTypes.h"
#include "GameplayTagContainer.h"
#include "Engine/DataAsset.h"
#include "Engine/DataTable.h"

#include "GameAbilities/GameplayEffectContexts.h"

#include "GameplayEffectPayloads.generated.h"

/**
 * 
 */

//#include "GameAbilities/GameplayEffectPayloads.h"

UENUM(BlueprintType)
enum class EPayloadTarget : uint8
{
    RuleSource,
    EventCauser,
    EventTarget,
    MAX UMETA(meta = (Hidden))
};

UENUM(BlueprintType)
enum class EReservedActionTiming : uint8
{
    AfterCurrentAction,
    StartOfNextTurn,
    EndOfTurn,
	MAX UMETA(meta = (Hidden))
};

UENUM(BlueprintType)
enum class EReservedActionType : uint8
{
    ExtraTurn,
    ExtraSkill,
    MAX UMETA(meta = (Hidden))
};

UCLASS(BlueprintType)
class DEMO_API UDA_ActionReservation : public UPrimaryDataAsset
{
    GENERATED_BODY()
public:
    UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, meta = (EditCondition = "Type == EReservedActionType::ExtraSkill", EditConditionHides))
        EPayloadTarget Instigator;

    UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
        EPayloadTarget Target;

    UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
        EReservedActionTiming Timing;

    UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
        EReservedActionType Type;

    UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, meta = (EditCondition = "Type == EReservedActionType::ExtraSkill", EditConditionHides))
        FGameplayTag SkillTag;
};

class UGameplayEffect;

UENUM(BlueprintType)
enum class EPayloadTargetTeamCondition : uint8
{
    All,
    Ally,
    Enemy,
    Self,
    MAX UMETA(meta = (Hidden))
};

UENUM(BlueprintType)
enum class EPayloadTargetSelectCondition : uint8
{
    All,
    Random,
    Highest,
    Lowest,
    MAX UMETA(meta = (Hidden))
};

UENUM(BlueprintType)
enum class EPayloadTargetAttributeCondition : uint8
{
    Health,
    MAX UMETA(meta = (Hidden))
};

UCLASS(BlueprintType)
class DEMO_API UDA_ApplyGE : public UPrimaryDataAsset
{
    GENERATED_BODY()
public:
    UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
        EPayloadTarget Target;

    UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
        EPayloadTargetTeamCondition TeamCondition;

    UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
        EPayloadTargetSelectCondition SelectCondition;

    UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, meta = (EditCondition = "SelectCondition == EPayloadTargetSelectCondition::Highest||SelectCondition == EPayloadTargetSelectCondition::Lowest", EditConditionHides))
        EPayloadTargetAttributeCondition AttributeCondition;

    UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, meta = (ClampMin = 1.00, EditCondition = "SelectCondition != EPayloadTargetSelectCondition::All", EditConditionHides))
        int32 TargetCount = 1;

    UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
        TSubclassOf<UGameplayEffect> GE;
};

UENUM(BlueprintType)
enum class EEffectEventResultType : uint8
{
    ReserveTurn,
    ReserveSkill,
    MAX UMETA(meta = (Hidden))
};

UENUM(BlueprintType)
enum class EEffectEventPhase : uint8
{
    Pre,
    Hit,
    MAX UMETA(meta = (Hidden))
};

UENUM(BlueprintType)
enum class EEffectEventTriggerCondition : uint8
{
    Always,
    TriggeredBySource,
    TriggeredByOther,
    MAX UMETA(meta = (Hidden))
};

USTRUCT(BlueprintType)
struct FEffectEventRule : public FTableRowBase
{
    GENERATED_BODY()
public:
    UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
        FGameplayTag TargetGrantedTag;

    UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
        FGameplayTag InstigatorGrantedTag;

    UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
        FPayloadContext TriggerContext;

    UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
        EEffectEventPhase EventPhase;

    UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
        EEffectEventTriggerCondition TriggerCondition;

    UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
        EEffectEventResultType Result;
};
