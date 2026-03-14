#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameplayTagContainer.h"
#include "GameAbilities/GameplayEffectContexts.h"
#include "DamageDealer.generated.h"

class AController;
class UShapeComponent;
class UGameplayEffect;
class UAbilityComponent;

USTRUCT(BlueprintType)
struct FDamageParameters
{
	GENERATED_BODY()
public:
	UPROPERTY(BlueprintReadOnly)float InstigatorPower;
	UPROPERTY(BlueprintReadOnly)float InstigatorSpeed;
	UPROPERTY(BlueprintReadOnly)float TargetDefense;
	UPROPERTY(BlueprintReadOnly)float TargetHealth;
	UPROPERTY(BlueprintReadOnly)float TargetMaxHealth;
	UPROPERTY(BlueprintReadOnly)bool bIsCritical;
};

UENUM(BlueprintType)
enum class EDamageTriggerRule : uint8
{
	NONE,
	Always,
	OnFirstHit,
	OnCriticalHit,
	OncePerTarget,
	MAX UMETA(meta = (Hidden))
};

USTRUCT(BlueprintType)
struct FDamageEffectData
{
	GENERATED_BODY()
public:
	UPROPERTY(EditDefaultsOnly)
		EDamageTriggerRule Rule;

	UPROPERTY(EditDefaultsOnly)
		TSubclassOf<UGameplayEffect> GEClass;

	UPROPERTY(EditDefaultsOnly)
		FGameplayTag GrantedTag;

	UPROPERTY(EditDefaultsOnly, meta = (ClampMin = 1.00))
		int32 StackCount;
};

UCLASS()
class DEMO_API ADamageDealer : public AActor
{
	GENERATED_BODY()
	
public:	
	ADamageDealer();
protected:
	virtual void BeginPlay() override;
public:	
	virtual void Tick(float DeltaTime) override;

	//property
private:
	bool bAct;
	bool bActivateEvent;
	bool bSendDamageEvent;
	bool bDeactivateEvent;
	bool bApplyNextCameraMove;
	bool bPlayNextMontage;
	bool bSpawnNextDamageDealer;
	int32 CurrentHitCount;
	FSpawnDamageDealerContext Data;
protected:
	float CurrentDamageTick;
	FHitResult HitResult;
protected:
	UPROPERTY(EditDefaultsOnly, Category = "Datas|Init")
		TSubclassOf<UGameplayEffect> DamageGEClass;

	UPROPERTY(EditDefaultsOnly, Category = "Datas|Init")
		int32 MaxHitCount = 1;

	UPROPERTY(EditDefaultsOnly, Category = "Datas|Init")
		float ActivateDelay;

	UPROPERTY(EditDefaultsOnly, Category = "Datas|Init")
		float DeactivateDelay;

	UPROPERTY(EditDefaultsOnly, Category = "Datas|Sequence")
		bool bUseDeactivateCameraMove;

	UPROPERTY(EditDefaultsOnly, Category = "Datas|Sequence", meta = (ClampMin = 0.00, EditCondition = "bUseDeactivateCameraMove", EditConditionHides))
		float CameraMoveDelay;

	UPROPERTY(EditDefaultsOnly, Category = "Datas|Sequence")
		bool bUseDeactivateMontage;

	UPROPERTY(EditDefaultsOnly, Category = "Datas|Sequence", meta = (ClampMin = 0.00, EditCondition = "bUseDeactivateMontage", EditConditionHides))
		float MontageDelay;

	UPROPERTY(EditDefaultsOnly, Category = "Datas|Sequence")
		bool bUseDeactivateSpawnDamageDealer;

	UPROPERTY(EditDefaultsOnly, Category = "Datas|Sequence", meta = (ClampMin = 0.00, EditCondition = "bUseDeactivateSpawnDamageDealer", EditConditionHides))
		float SpawnDamageDealerDelay;

	UPROPERTY(EditDefaultsOnly, Category = "Datas|Sequence", meta = (ClampMin = 0.10))
		float DamageTick = 0.1;	



	/*
	* activate,deactivate effect 추가해야됨
	* hiteffect
	*/

	UPROPERTY(EditDefaultsOnly, Category = "Datas|Ability")
		FDamageEffectData AdditiveEffectData;
public:

	//function
private:
protected:
	virtual void SendDamage(AActor* Target, const FHitResult& SweepResult);
	virtual void ApplyDamageGE(UAbilityComponent* InstigatorASC, UAbilityComponent* TargetASC, const FHitResult& SweepResult);
	virtual void ApplyAdditiveEffectData(UAbilityComponent* InstigatorASC, UAbilityComponent* TargetASC, const FHitResult& SweepResult);
	virtual void Activate();
	virtual void ApplyNextCameraMove();
	virtual void PlayNextMontage();
	virtual void SpawnNextDamageDealer();
	virtual void Deactivate();
	virtual bool TryDestroy();

	void SendEvent(const TArray<FDamageDealerTriggerData>& InDatas,bool& InFlag);

	FORCEINLINE const FSpawnDamageDealerContext& GetDataContext()const { return Data; }
public:
	virtual void Init(const FSpawnDamageDealerContext* InData);

	FORCEINLINE bool IsActivated()const { return bAct; }
};

/*
* 
* 블루프린트 단계에서 추가된 충돌 컴포넌트의 태그에 OverlapComponentTag를 넣어 사용
* TODO::타이머적용
* 데미지 계산할때 외부에서 계수를 가져올건지? 내부계수는 고정이고 추가 강화 시스템을 가져올건지?
* 
*/