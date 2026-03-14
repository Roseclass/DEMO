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

UENUM(BlueprintType)    
enum class ECameraShotType : uint8
{
    OTS_Right,          // 시전자 오른쪽 어깨 뒤편
    OTS_Left,           // 시전자 왼쪽 어깨 뒤편
    Duel_TwoShot,       // 1:1 대결 구도
    Mid_Front3Q,        // 인물 중심
    Close_Up,           // 확대
    Impact_Close,       // 히트 순간 확대
    Wide_Establish,     // 전장/다수 타겟/턴 시작 와이드
    Group_3Q,           // 3~4명 프레이밍
    TopDown_Tactical,   // 전술/배치 강조(위에서 내려다봄)
    LowAngle_Finisher,  // 피니시/킬샷(낮게, 위로 올려다봄)
    Side_Profile,       // 횡이동/라인액션(측면 구도)
    Rear_Wide,          // 뒤에서 크게 잡아 진입/돌진
    MAX UMETA(Hidden)
};

UENUM(BlueprintType)
enum class ECameraLookAtType : uint8    // 어디방향 쳐다볼거임?
{
    Target_Primary,     // TargetActors[0]
    Target_RandomOnce,  // Shot 시작 시 1회 랜덤
    Target_Center,      // TargetActors 중심점
    ImpactPoint,        // HitResult.ImpactPoint
    Origin_Forward,     // ShotOriginActor 정면
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
    UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
        ECameraShotType ShotType;

    UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
        ECameraLookAtType LookAtType;

    UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
        ECameraEventType EventType;

    UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
        ECameraSkillType SkillType;

    TWeakObjectPtr<AActor> ShotOriginActor;
    TArray<TWeakObjectPtr<AActor>> TargetActors;

    /*
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
    */
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
    UPROPERTY()FVector Location;
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
