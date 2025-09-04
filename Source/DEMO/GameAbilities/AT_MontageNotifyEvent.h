#pragma once

#include "CoreMinimal.h"
#include "Abilities/Tasks/AbilityTask.h"
#include "GameAbilities/AbilityTaskTypes.h"
#include "AT_MontageNotifyEvent.generated.h"

/*
* 어빌리티의 타이머, 람다함수같은 역할
* 몽타주 노티파이로 전송받은 이벤트 태그를 활성화된 어빌리티 전부에 찔러보고 일치하는지 검사하고 이벤트를 실행
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
	UAnimMontage* MontageToPlay;             // 재생할 몽타주
	FGameplayTagContainer EventTags;         // 게임플레이 이벤트와 매칭할 태그 목록
	float Rate;                              // 재생 속도
	FName StartSection;                      // 몽타주를 시작할 섹션 이름
	float AnimRootMotionTranslationScale;    // 루트 모션 이동에 적용할 배율
	bool bStopWhenAbilityEnds;               // 어빌리티가 종료될 때 몽타주를 중지할지 여부
	FOnMontageBlendingOutStarted BlendingOutDelegate;
	FOnMontageEnded MontageEndedDelegate;
	FDelegateHandle CancelledHandle;
	FDelegateHandle EventHandle;
protected:
public:
	FAbilityTaskEventDelegate OnCompleted;   // 몽타주가 완전히 재생을 마쳤을 때
	FAbilityTaskEventDelegate OnBlendOut;    // 몽타주가 블렌드 아웃을 시작했을 때
	FAbilityTaskEventDelegate OnInterrupted; // 몽타주가 인터럽트되었을 때
	FAbilityTaskEventDelegate OnCancelled;   // 다른 어빌리티에 의해 명시적으로 태스크가 취소되었을 때
	FAbilityTaskEventDelegate EventReceived; // 트리거 조건 중 하나인 게임플레이 이벤트가 발생했을 때

	//function
private:
	UAbilityComponent* GetTargetAbilityComp();
	void OnMontageBlendingOut(UAnimMontage* Montage, bool bInterrupted);
	void OnAbilityCancelled();
	void OnMontageEnded(UAnimMontage* Montage, bool bInterrupted);
	void OnGameplayEvent(FGameplayTag EventTag, const FGameplayEventData* Payload);
	bool StopPlayingMontage();	// 어빌리티가 몽타주를 재생 중인지 확인하고, 재생 중이라면 몽타주를 중지시킨다. 몽타주가 중지되었으면 true, 그렇지 않으면 false를 반환한다.
protected:
public:

};