#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "GameAbilities/AbilityTypes.h"
#include "AbilityComponent.generated.h"

/**
 * DEMO 프로젝트에서 사용할 ASC
 */

class UGA_BaseAbility;
class ATurnBasedCharacter;
struct FAttributeInitialInfos;

DECLARE_MULTICAST_DELEGATE(FAbilityComponentSignature);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnRegisterPendingDeadArray, AActor*);

UCLASS()
class DEMO_API UAbilityComponent : public UAbilitySystemComponent
{
	GENERATED_BODY()
public:
	UAbilityComponent();
protected:
	virtual void BeginPlay() override;
public:
	virtual FActiveGameplayEffectHandle ApplyGameplayEffectSpecToSelf(const FGameplayEffectSpec& GameplayEffect, FPredictionKey PredictionKey = FPredictionKey())override;

	//property
private:
	TArray<ATurnBasedCharacter*> Targets;

	FGameplayTag StateTag_Dead;
	FGameplayTag StateTag_Hit;
	FGameplayTag StateTag_StunStart;
	FGameplayTag StateTag_StunEnd;

	TSet<FGameplayAbilitySpecHandle> GAHandles;
	TMap<FGameplayTag, FGameplayAbilitySpecHandle> StateGAHandles;
	TMap<FGameplayTag, FGameplayAbilitySpecHandle> SkillGAHandles;
	FGameplayTagContainer CooldownTags;
	TSet<FActiveGameplayEffectHandle> CooldownHandles;
	TSet<FActiveGameplayEffectHandle> BuffHandles;
	TSet<FActiveGameplayEffectHandle> DebuffHandles;
	TSet<FActiveGameplayEffectHandle> CCHandles;
	TSet<FActiveGameplayEffectHandle> DoTDamageHandles;

	TMap<FGameplayTag, FActiveGameplayEffectHandle> DurationFXHandles;
protected:
public:
	FAbilityComponentSignature OnSkillEnd;
	FAbilityComponentSignature OnDeadSequenceEnd;
	FOnRegisterPendingDeadArray OnRegisterPendingDeadArray;

	//function
private:
	UFUNCTION()void OnAnyGameplayEffectRemoved(const FActiveGameplayEffect& RemovedGE);
protected:
public:
	void InitGA(const TArray<FAbilitySpecInfo>& NewGAs);
	void InitAttributes(const FAttributeInitialInfos* NewStats);

	//with TurnBased
	void RegisterPendingDeadArray();
	void BeginDeadAbility();
	void BeginHitAbility();
	void EndStunAbility();
	void BroadcastOnSkillEnd();
	void BroadcastOnDeadSequenceEnd();
	void GetSkillCooldownTimeRemainingAndDuration(FGameplayTag InSkillTag, OUT float& TimeRemaining, OUT float& CooldownDuration);
	FGameplayAbilitySpec* FindAbilitySpecFromSkillTag(FGameplayTag InSkillTag);

	void GetAllDoTDamageHandles(OUT TArray<FActiveGameplayEffectHandle>& OutHandles);
	void HandleCooldown();
	void HandleBuff();
	void HandleDebuff();
	void HandleCC();
	void HandleDoTDamage(FActiveGameplayEffectHandle InHandle, OUT AActor** EventCauserActor, OUT AActor** EventTargetActor);

	bool HasTurnBlockingCC();

	float GetHealth() const;
	float GetMaxHealth() const;
	float GetDefense() const;
	float GetPower() const;
	float GetSpeed() const;
	float GetTurnGauge() const;

	FORCEINLINE void SetTargets(TArray<ATurnBasedCharacter*> InTargets) { Targets = InTargets; }
	FORCEINLINE TArray<ATurnBasedCharacter*> GetTargets() const { return Targets; }
};


//
// 태그 설정
// 캐릭터 리타게팅 및 쓸만한 스킬시퀀스 찾기
//