#pragma once

#include "CoreMinimal.h"
#include "Abilities/Tasks/AbilityTask.h"
#include "GameAbilities/AbilityTaskTypes.h"
#include "AT_MontageNotifyEvent.generated.h"

/**
 * 
 */

class UAnimMontage;
class UGameplayAbility;
class UAbilityComponent;


UCLASS()
class DEMO_API UAT_MontageNotifyEvent : public UAbilityTask
{
	GENERATED_BODY()
public:
	UAT_MontageNotifyEvent();
	UFUNCTION(BlueprintCallable, Category = "Ability|Tasks", meta = (HidePin = "OwningAbility", DefaultToSelf = "OwningAbility", BlueprintInternalUseOnly = "TRUE"))
	static UAT_MontageNotifyEvent* CreateMontageNotifyEvent(UGameplayAbility* OwningAbility,FName TaskInstanceName,	UAnimMontage* MontageToPlay,FGameplayTagContainer EventTags,
		float Rate = 1.f,FName StartSection = NAME_None,bool bStopWhenAbilityEnds = true,float AnimRootMotionTranslationScale = 1.f);
protected:
public:
	virtual void Activate() override;
	virtual void ExternalCancel() override;
	virtual void OnDestroy(bool AbilityEnded) override;

	//property
private:
	UAnimMontage* MontageToPlay;			//Montage that is playing
	FGameplayTagContainer EventTags;		//List of tags to match against gameplay events
	float Rate;								//Playback rate
	FName StartSection;						//Section to start montage from
	float AnimRootMotionTranslationScale;	//Modifies how root motion movement to apply
	bool bStopWhenAbilityEnds;				//Rather montage should be aborted if ability ends
	FOnMontageBlendingOutStarted BlendingOutDelegate;
	FOnMontageEnded MontageEndedDelegate;
	FDelegateHandle CancelledHandle;
	FDelegateHandle EventHandle;
protected:
public:
	FAbilityTaskEventDelegate OnCompleted;	//The montage completely finished playing
	FAbilityTaskEventDelegate OnBlendOut;	//The montage started blending out
	FAbilityTaskEventDelegate OnInterrupted;//The montage was interrupted
	FAbilityTaskEventDelegate OnCancelled;	//The ability task was explicitly cancelled by another ability
	FAbilityTaskEventDelegate EventReceived;//One of the triggering gameplay events happened

	//function
private:
	UAbilityComponent* GetTargetAbilityComp();
	void OnMontageBlendingOut(UAnimMontage* Montage, bool bInterrupted);
	void OnAbilityCancelled();
	void OnMontageEnded(UAnimMontage* Montage, bool bInterrupted);
	void OnGameplayEvent(FGameplayTag EventTag, const FGameplayEventData* Payload);
	bool StopPlayingMontage();	//Checks if the ability is playing a montage and stops that montage, returns true if a montage was stopped, false if not.
protected:
public:

};

/*
* 어빌리티의 타이머, 람다함수같은 역할
* 노티파이로 전송받은 이벤트 태그를 활성화된 어빌리티 전부에 찔러보고 일치하는지 검사하고 이벤트를 실행
*/