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
enum class EPayloadActorType : uint8
{
    RuleSource,
    EventCauser,
    EventTarget,
    EventTargets,
    MAX UMETA(meta = (Hidden))
};

////
//// Reserve Action
////

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
        EPayloadActorType Instigator;

    UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
        EPayloadActorType Target;

    UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
        EReservedActionTiming Timing;

    UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
        EReservedActionType Type;

    UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, meta = (EditCondition = "Type == EReservedActionType::ExtraSkill", EditConditionHides))
        FGameplayTag SkillTag;
};

////
//// ApplyGE
////

class UGameplayEffect;

UENUM(BlueprintType)
enum class EPayloadReferenceTeamCondition : uint8
{
    All,
    Ally,
    Enemy,
    Self,
    MAX UMETA(meta = (Hidden))
};

UENUM(BlueprintType)
enum class EPayloadReferenceSelectCondition : uint8
{
    All,
    Random,
    Highest,
    Lowest,
    MAX UMETA(meta = (Hidden))
};

UENUM(BlueprintType)
enum class EPayloadReferenceAttributeCondition : uint8
{
    Health,
    MAX UMETA(meta = (Hidden))
};

UCLASS(BlueprintType)
class DEMO_API UDA_ApplyGE : public UPrimaryDataAsset
{
    GENERATED_BODY()
public:
    UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Condition")
        EPayloadActorType ReferenceActor;

    UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Condition")
        EPayloadReferenceTeamCondition TeamCondition;

    UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Condition", meta = (EditCondition = "TeamCondition != EPayloadReferenceTeamCondition::Self", EditConditionHides))
        EPayloadReferenceSelectCondition SelectCondition;

    UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Condition", meta = (EditCondition = "SelectCondition == EPayloadReferenceSelectCondition::Highest||SelectCondition == EPayloadReferenceSelectCondition::Lowest", EditConditionHides))
        EPayloadReferenceAttributeCondition AttributeCondition;

    UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Condition", meta = (ClampMin = 1.00, EditCondition = "SelectCondition != EPayloadReferenceSelectCondition::All", EditConditionHides))
        int32 GoalCount = 1;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "GE")
        TSubclassOf<UGameplayEffect> GE;

    UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "GE")
        int32 StackCount = 1;
};

////
//// Move Camera
////

UENUM(BlueprintType)
enum class ECameraShotType : uint8
{
    // 시전자 오른쪽 어깨 뒤편
    OTS_Right,          
    // 시전자 왼쪽 어깨 뒤편
    OTS_Left,           
    // 1:1 대결 구도
    Duel_TwoShot,  
    // 인물 중심
    Mid_Front3Q,     
    // 확대
    Close_Up,       
    // 히트 순간 확대
    Impact_Close,       
    // 전장/다수 타겟/턴 시작 와이드
    Wide_Establish,     
    // 3~4명 프레이밍
    Group_3Q,           
    // 전술/배치 강조(위에서 내려다봄)
    TopDown_Tactical,   
    // 피니시/킬샷(낮게, 위로 올려다봄)
    LowAngle_Finisher,  
    // 횡이동/라인액션(측면 구도)
    Side_Profile,       
    // 뒤에서 크게 잡아 진입/돌진
    Rear_Wide,          
    MAX UMETA(Hidden)
};

UENUM(BlueprintType)
enum class ECameraLookAtType : uint8    // 어디방향 쳐다볼거임?
{
    // TargetActors[0]
    Target_Primary,     
    // Shot 시작 시 1회 랜덤
    Target_RandomOnce,  
    // TargetActors 중심점
    Target_Center,      
    // HitResult.ImpactPoint
    ImpactPoint,        
    // ShotOriginActor 정면
    Origin_Forward,     
    MAX UMETA(Hidden)
};

UENUM(BlueprintType)
enum class ECameraEventType : uint8 // 무슨 이벤트임 ?
{
    TurnStart,
    ActionStart,
    HitConfirmed,
    Explosion,
    Interrupt,
    Death,
    ActionEnd,
    MAX UMETA(Hidden)
};

UENUM(BlueprintType)
enum class ECameraSkillType : uint8    // 어떤 타입의 이벤트임?
{
    Basic,
    Single,
    MultiHit,
    AoE,
    Projectile,
    Dash,
    Protect,
    Counter,
    Buff,
    Debuff,
    MAX UMETA(Hidden)
};

UCLASS(BlueprintType)
class DEMO_API UDA_MoveCamera : public UPrimaryDataAsset
{
    GENERATED_BODY()
public:
    UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
        EPayloadActorType ShotOrigin;

    UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
        ECameraShotType ShotType;

    UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
        ECameraLookAtType LookAtType;

    UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
        ECameraEventType EventType;

    UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
        ECameraSkillType SkillType;
};

////
//// Effect Event Rule
////

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
