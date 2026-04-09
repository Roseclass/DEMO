#pragma once

#include "CoreMinimal.h"
#include "UObject/ObjectMacros.h"
#include "UObject/Object.h"
#include "GameplayEffectTypes.h"
#include "GameplayTagContainer.h"
#include "Engine/DataAsset.h"
#include "Engine/DataTable.h"

//#include "GameAbilities/GameplayEffectPayloads.h"

#include "GameplayEffectPayloads.generated.h"

/**
 * 
 */

UENUM(BlueprintType)
enum class EPayloadActorType : uint8
{
    RuleSource,
    EventCauser,
    EventTargets,
    MAX UMETA(meta = (Hidden))
};

UENUM(BlueprintType)
enum class EPayloadTargetSelectType  : uint8
{
    Random,
    First,
    Center,
    Last,
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


USTRUCT(BlueprintType)
struct FApplyGEData
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
    ActorToActor,
    ActorToLoc,
    LocToActor,
    LocToLoc,
    MAX UMETA(Hidden)
};

UENUM(BlueprintType)
enum class ECameraLookAtType : uint8    // 어디방향 쳐다볼거임?
{
    OriginToDest,
    DestToOrigin,
    MAX UMETA(Hidden)
};

class UCurveFloat;
class UCameraShakeBase;

USTRUCT(BlueprintType)
struct FMoveCameraData
{
    GENERATED_BODY()
public:
    UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
        ECameraShotType ShotType;

    UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
        ECameraLookAtType LookAtType;

    UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, meta = (EditCondition = "ShotType == ECameraShotType::ActorToActor || ShotType == ECameraShotType::ActorToLoc", EditConditionHides))
        EPayloadActorType OriginActor;

    UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, meta = (EditCondition = "OriginActor == EPayloadActorType::EventTargets", EditConditionHides))
        EPayloadTargetSelectType OriginActorSelectType;

    UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, meta = (EditCondition = "ShotType == ECameraShotType::ActorToActor || ShotType == ECameraShotType::ActorToLoc", EditConditionHides))
        bool bAttach;

    UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, meta = (EditCondition = "bAttach", EditConditionHides))
        FName AttachSocketName;

    UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, meta = (EditCondition = "bAttach", EditConditionHides))
        FVector AttachOffset;

    UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, meta = (EditCondition = "!bAttach", EditConditionHides))
        FVector OriginActorOffset; // x means forward offset, y means right offset, z means up offset

    UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, meta = (EditCondition = "ShotType == ECameraShotType::LocToActor || ShotType == ECameraShotType::LocToLoc", EditConditionHides))
        FVector OriginLoc;

    UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, meta = (EditCondition = "ShotType == ECameraShotType::ActorToActor || ShotType == ECameraShotType::LocToActor", EditConditionHides))
        EPayloadActorType DestActor;

    UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, meta = (EditCondition = "DestActor == EPayloadActorType::EventTargets", EditConditionHides))
        EPayloadTargetSelectType DestActorSelectType;

    UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, meta = (EditCondition = "ShotType == ECameraShotType::ActorToActor || ShotType == ECameraShotType::LocToActor", EditConditionHides))
        FVector DestActorOffset; // x means forward offset, y means right offset, z means up offset

    UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, meta = (EditCondition = "ShotType == ECameraShotType::ActorToLoc || ShotType == ECameraShotType::LocToLoc", EditConditionHides))
        FVector DestLoc;

    UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
        FRuntimeFloatCurve SpringArmCurve;

    UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
        float LBlendStart;

    UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
        float LBlendEnd;

    UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
        float RBlendStart;

    UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
        float RBlendEnd;

    UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
        float CameraLagSpeed = 10;

    float ElapsedTime = 0;
};

/*
* 
    UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
        UCurveFloat* CloseUpCurve;

    UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
        TSubclassOf<UCameraShakeBase> CameraShake;

* 공통
* 스킬 선택 정면샷, 타겟 선택 와이드뷰, 인포 정리 항공뷰
* 
* revenant
* attack execution + cone randomize angle + fadeout
* cast impact  + cone randomize angle + fadeout
* castmassive(미완)
* castult followcam->impact + fadeout
* 
* 일단은 카메라를 내가 원하는 위치에 원하는 방향을 바라보게 보내는것부터 먼저하자.
* 
*/

USTRUCT(BlueprintType)
struct FReserveActionData
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
struct FSpawnDamageDealerData
{
    GENERATED_BODY()
public:
    TWeakObjectPtr<AActor> TargetActor;

    UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
        TSubclassOf<ADamageDealer> Class;

    UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
        FName SocketName;

    UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
        float DelayMin;

    UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
        float DelayMax;

    UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
        bool bIsHoming;

    UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, meta = (EditCondition = "bIsHoming", EditConditionHides))
        FRotator SpawnDirectionOffset;

    UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, meta = (EditCondition = "bIsHoming", EditConditionHides))
        float SpawnAngle;

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
struct FChangeTargetData
{
    GENERATED_BODY()
public:
    UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
        EPayloadActorType PreTarget;
};

UENUM(BlueprintType)
enum class EScriptedMoveLocationSourceType : uint8
{
    Actor, Location,
    MAX UMETA(Hidden)
};

UENUM(BlueprintType)
enum class EScriptedMoveRotationSourceType : uint8
{
    CopyActor, LookAtActor, Rotation,
    MAX UMETA(Hidden)
};

USTRUCT(BlueprintType)
struct FScriptedMoveData
{
    GENERATED_BODY()
public:
    UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
        bool bReturn;

    UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
        float Duration;
    float ElapsedTime;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Location")
        float LocationStart;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Location")
        float LocationEnd;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Location", meta = (EditCondition = "!bReturn", EditConditionHides))
        EScriptedMoveLocationSourceType LocationSourceType;

    UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Location", meta = (EditCondition = "LocationSourceType == EScriptedMoveLocationSourceType::Actor && !bReturn", EditConditionHides))
        EPayloadActorType LocationActorType;

    UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Location", meta = (EditCondition = "LocationSourceType == EScriptedMoveLocationSourceType::Actor && LocationActorType == EPayloadActorType::EventTargets && !bReturn", EditConditionHides))
        EPayloadTargetSelectType LocationActorSelectType;

    UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Location", meta = (EditCondition = "LocationSourceType == EScriptedMoveLocationSourceType::Actor && !bReturn", EditConditionHides))
        float FrontOffset;

    UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Location", meta = (EditCondition = "LocationSourceType == EScriptedMoveLocationSourceType::Actor && !bReturn", EditConditionHides))
        float RightOffset;

    UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Location", meta = (EditCondition = "LocationSourceType == EScriptedMoveLocationSourceType::Actor && !bReturn", EditConditionHides))
        float UpOffset;

    UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Location", meta = (EditCondition = "LocationSourceType == EScriptedMoveLocationSourceType::Location && !bReturn", EditConditionHides))
        FVector TargetLocation;

    bool LocationInit;
    float LocationSpeed;
    TWeakObjectPtr<AActor> LocationActor;

    UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Rotation")
        float RotationStart;

    UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Rotation")
        float RotationEnd;

    UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Rotation", meta = (EditCondition = "!bReturn", EditConditionHides))
        EScriptedMoveRotationSourceType RotationSourceType;

    UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Rotation", meta = (EditCondition = "RotationSourceType != EScriptedMoveRotationSourceType::Rotation && !bReturn", EditConditionHides))
        EPayloadActorType RotationActorType;

    UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Rotation", meta = (EditCondition = "RotationSourceType == EScriptedMoveRotationSourceType::CopyActor && !bReturn", EditConditionHides))
        FRotator AdditionalRotation;

    UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Rotation", meta = (EditCondition = "RotationSourceType == EScriptedMoveRotationSourceType::LookAtActor && !bReturn", EditConditionHides))
        FVector LookAtOffset;

    UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Rotation", meta = (EditCondition = "RotationSourceType != EScriptedMoveRotationSourceType::Rotation && RotationActorType == EPayloadActorType::EventTargets && !bReturn", EditConditionHides))
        EPayloadTargetSelectType RotationActorSelectType;

    UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Rotation", meta = (EditCondition = "RotationSourceType == EScriptedMoveRotationSourceType::Rotation && !bReturn", EditConditionHides))
        FRotator TargetRotation;

    bool RotationInit;
    FRotator RotationDeltaPerSecond;
    TWeakObjectPtr<AActor> RotationActor;

    UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Jump")
        bool bUseJump;

    UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Jump", meta = (EditCondition = "bUseJump", EditConditionHides))
        float JumpStart;

    bool JumpInit;
public:
    void SetReturn(const FTransform& OriginTransform)
    {
        if (!bReturn)return;
        LocationSourceType = EScriptedMoveLocationSourceType::Location;
        TargetLocation = OriginTransform.GetTranslation();
        RotationSourceType = EScriptedMoveRotationSourceType::Rotation;
        TargetRotation = OriginTransform.GetRotation().Rotator();
    }
};

UCLASS(BlueprintType)
class DEMO_API UDA_GCNPayload : public UPrimaryDataAsset
{
    GENERATED_BODY()
public:
    UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
        TArray<FApplyGEData>ApplyGE;

    UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
        TArray<FMoveCameraData>MoveCamera;

    UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
        TArray<FReserveActionData>ReserveAction;

    UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
        TArray<FSpawnDamageDealerData>SpawnDamageDealer;

    UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
        TArray<FChangeTargetData> ChangeTarget;

    UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
        TArray<FScriptedMoveData> ScriptedMove;
};
