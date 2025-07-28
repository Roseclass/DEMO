#pragma once

#include "CoreMinimal.h"
#include "GameAbilities/GA_MontageWithEvent.h"
#include "Engine/NetSerialization.h"
#include "GA_Skill.generated.h"

/**
 *
 */

class UAnimMontage;
class ADamageDealer;
class AWarningSign;

USTRUCT(BlueprintType)
struct FDamageDealerData
{
	GENERATED_BODY()
public:
	//UPROPERTY(EditAnywhere)
	//	TSubclassOf<ADamageDealer> Class;

	UPROPERTY(EditAnywhere)
		FName SocketName;

	UPROPERTY(EditAnywhere)
		bool bUseSocketLocation = 1;

	UPROPERTY(EditAnywhere)
		bool bUseSocketRotation = 0;

	UPROPERTY(EditAnywhere, meta = (EditCondition = "!bUseSocketLocation", EditConditionHides))
		float FrontDist;

	UPROPERTY(EditAnywhere, meta = (EditCondition = "!bUseSocketLocation", EditConditionHides))
		float RightDist;

	UPROPERTY(EditAnywhere, meta = (EditCondition = "!bUseSocketLocation", EditConditionHides))
		float UpDist;

	UPROPERTY(EditAnywhere, meta = (EditCondition = "!bUseSocketRotation", EditConditionHides))
		FRotator Rotation;
};

USTRUCT(BlueprintType)
struct FWaringSignData
{
	GENERATED_BODY()
public:
	//UPROPERTY(EditAnywhere)
	//	TSubclassOf<AWarningSign> WarningSignClass;

	UPROPERTY(EditAnywhere)
		float ForwardOffset = 0;

	UPROPERTY(EditAnywhere)
		float RightOffset = 0;

	UPROPERTY(EditAnywhere)
		FVector Scale = FVector(1, 1, 1);

	UPROPERTY(EditAnywhere)
		float Duration;

	UPROPERTY(EditAnywhere)
		float ExtraDuration;
};

USTRUCT(BlueprintType)
struct FDamageEhancementData
{
	GENERATED_BODY()
public:
	UPROPERTY(EditDefaultsOnly)
		FGameplayTag AdditiveTag;

	UPROPERTY(EditDefaultsOnly)
		FGameplayTag MultiplicitiveTag;

	UPROPERTY()
		float Additive;

	UPROPERTY()
		float Multiplicitive = 100;
};

UCLASS()
class DEMO_API UGA_Skill : public UGA_MontageWithEvent
{
	GENERATED_BODY()
public:
	UGA_Skill();
protected:
public:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	virtual void ApplyCooldown(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo) const override;
	virtual const FGameplayTagContainer* GetCooldownTags() const override;
	virtual bool CheckCost(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, OUT FGameplayTagContainer* OptionalRelevantTags = nullptr) const override;
	virtual void ApplyCost(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo) const override;

	//property
private:
protected:
	UPROPERTY(EditAnywhere, Category = "Dealer")
		TMap<FGameplayTag, FDamageDealerData> DamageDealerDataMap;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Damage")
		FDamageEhancementData DamageData;

	UPROPERTY(EditAnywhere, Category = "Condition|Range")
		float Range = 500.0f;

	UPROPERTY(EditAnywhere, Category = "Condition|Cost")
		FScalableFloat CostBase;

	UPROPERTY(EditAnywhere, Category = "Condition|Cooldown")
		FScalableFloat CooldownBase;

	UPROPERTY(EditAnywhere, Category = "WarningSign")
		TMap<FGameplayTag, FWaringSignData> WarningSignDataMap;

	UPROPERTY(Transient)
		FGameplayTagContainer TempCooldownTags;
public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Tags, meta = (Categories = "AbilityTagCategory"))
		FGameplayTagContainer CooldownTags;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Tags, meta = (Categories = "AbilityTagCategory"))
		FGameplayTag EnhancementTag;

	//function
private:
	virtual void SpawnDamageDealer(FGameplayTag EventTag);
	virtual void SpawnWarningSign(FGameplayTag EventTag);
	virtual void OnCollision();
	virtual void OffCollision();
	virtual void ResetHitActors();
protected:
	virtual void EventReceived(FGameplayTag EventTag, FGameplayEventData EventData)override;

	virtual float GetCooldown(const FGameplayAbilityActorInfo* ActorInfo)const;
	virtual float GetCost(const FGameplayAbilityActorInfo* ActorInfo)const;
public:
};

/*
* 주석부분 다시 채우기
* 태그에 skill.@@@.Sequence1 같은 식으로 받고
* 마지막 부분을 인덱스로 스킬시퀀스를 진행하는 방식으로하자
*/