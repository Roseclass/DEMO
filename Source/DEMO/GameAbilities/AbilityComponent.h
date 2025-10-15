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
struct FAttributeInitialInfo;

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

	//property
private:
	ATurnBasedCharacter* Target;
protected:
public:
	FAbilityComponentSignature OnSkillEnd;
	FAbilityComponentSignature OnDeadSequenceEnd;

	//function
private:
protected:
public:
	void InitGA(const TArray<FAbilitySpecInfo>& NewGAs);
	void InitAttributes(const FAttributeInitialInfo* NewStats);
	void PlayDeadSequence();
	void BroadcastOnSkillEnd();
	void BroadcastOnDeadSequenceEnd();

	float GetHealth() const;
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