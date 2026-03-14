#pragma once

#include "CoreMinimal.h"
#include "Abilities/Tasks/AbilityTask.h"
#include "GameAbilities/AbilityTaskTypes.h"
#include "AT_SkillNotifyEvent.generated.h"

/**
 * 
 */

class UAnimMontage;
class UGameplayAbility;
class UAbilityComponent;

UCLASS()
class DEMO_API UAT_SkillNotifyEvent : public UAbilityTask
{
	GENERATED_BODY()
public:
	UAT_SkillNotifyEvent();
	UFUNCTION(BlueprintCallable, Category = "Ability|Tasks", meta = (HidePin = "OwningAbility", DefaultToSelf = "OwningAbility", BlueprintInternalUseOnly = "TRUE"))
		static UAT_SkillNotifyEvent* CreateSkillNotifyEvent(UGameplayAbility* OwningAbility, FName TaskInstanceName, FGameplayTagContainer EventTags);
protected:
public:
	virtual void Activate() override;
	virtual void OnDestroy(bool AbilityEnded) override;

	//property
private:
	FGameplayTagContainer EventTags;         // 게임플레이 이벤트와 매칭할 태그 목록
	FDelegateHandle EventHandle;
	FDelegateHandle EndHandle;
protected:
public:
	FAbilityTaskEventDelegate EventReceived; // 트리거 조건 중 하나인 게임플레이 이벤트가 발생했을 때

	//function
private:
	UAbilityComponent* GetTargetAbilityComp();
	void OnGameplayEvent(FGameplayTag EventTag, const FGameplayEventData* Payload);
protected:
public:

};
