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
	ATurnBasedCharacter* Target;
	TSet<FActiveGameplayEffectHandle> DoTDamageHandles;
protected:
public:
	FAbilityComponentSignature OnSkillEnd;
	FAbilityComponentSignature OnDeadSequenceEnd;

	//function
private:
protected:
public:
	void InitGA(const TArray<FAbilitySpecInfo>& NewGAs);
	void InitAttributes(const FAttributeInitialInfos* NewStats);
	void PlayDeadSequence();
	void BroadcastOnSkillEnd();
	void BroadcastOnDeadSequenceEnd();

	//with TurnBased
	void GetAllDoTDamageHandles(OUT TArray<FActiveGameplayEffectHandle>& OutHandles);
	void HandleDoTDamage(FActiveGameplayEffectHandle InHandle, OUT AActor** EventCauserActor, OUT AActor** EventTargetActor);

	float GetHealth() const;
	float GetMaxHealth() const;
	float GetDefense() const;
	float GetPower() const;
	float GetSpeed() const;
	float GetTurnGauge() const;

	FORCEINLINE void SetTarget(ATurnBasedCharacter* InTarget) { Target = InTarget; }
	FORCEINLINE ATurnBasedCharacter* GetTarget() const { return Target; }
};


//
// 태그 설정
// 캐릭터 리타게팅 및 쓸만한 스킬시퀀스 찾기
//