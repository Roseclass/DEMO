#include "GameAbilities/AT_MontageNotifyEvent.h"
#include "Global.h"
#include "AbilitySystemGlobals.h"
#include "Animation/AnimInstance.h"
#include "GameFramework/Character.h"

#include "GameAbilities/AbilityComponent.h"

UAT_MontageNotifyEvent::UAT_MontageNotifyEvent()
{
	Rate = 1.f;
	bStopWhenAbilityEnds = true;
}

UAT_MontageNotifyEvent* UAT_MontageNotifyEvent::CreateMontageNotifyEvent(UGameplayAbility* OwningAbility, FName TaskInstanceName, UAnimMontage* MontageToPlay, FGameplayTagContainer EventTags, float Rate, FName StartSection, bool bStopWhenAbilityEnds, float AnimRootMotionTranslationScale)
{
	UAbilitySystemGlobals::NonShipping_ApplyGlobalAbilityScaler_Rate(Rate);

	UAT_MontageNotifyEvent* result = NewAbilityTask<UAT_MontageNotifyEvent>(OwningAbility, TaskInstanceName);
	result->MontageToPlay = MontageToPlay;
	result->EventTags = EventTags;
	result->Rate = Rate;
	result->StartSection = StartSection;
	result->AnimRootMotionTranslationScale = AnimRootMotionTranslationScale;
	result->bStopWhenAbilityEnds = bStopWhenAbilityEnds;

	return result;
}

void UAT_MontageNotifyEvent::Activate()
{
	CheckNull(Ability);

	bool bPlayedMontage = false;
	UAbilityComponent* ASC = GetTargetAbilityComp();

	if (ASC)
	{
		const FGameplayAbilityActorInfo* ActorInfo = Ability->GetCurrentActorInfo();
		UAnimInstance* AnimInstance = ActorInfo->GetAnimInstance();
		if (AnimInstance != nullptr)
		{
			// 이벤트 콜백에 바인딩한다
			EventHandle = ASC->AddGameplayEventTagContainerDelegate(EventTags, FGameplayEventTagMulticastDelegate::FDelegate::CreateUObject(this, &UAT_MontageNotifyEvent::OnGameplayEvent));

			if (ASC->PlayMontage(Ability, Ability->GetCurrentActivationInfo(), MontageToPlay, Rate, StartSection) > 0.f)
			{
				// 몽타주를 재생하면 게임 코드 쪽으로 콜백이 발생할 수 있고, 이로 인해 이 어빌리티가 종료될 수도 있다!
				// Pending Kill 상태라면 조기에 함수 실행을 중단한다
				if (ShouldBroadcastAbilityTaskDelegates() == false)
				{
					return;
				}

				CancelledHandle = Ability->OnGameplayAbilityCancelled.AddUObject(this, &UAT_MontageNotifyEvent::OnAbilityCancelled);

				BlendingOutDelegate.BindUObject(this, &UAT_MontageNotifyEvent::OnMontageBlendingOut);
				AnimInstance->Montage_SetBlendingOutDelegate(BlendingOutDelegate, MontageToPlay);

				MontageEndedDelegate.BindUObject(this, &UAT_MontageNotifyEvent::OnMontageEnded);
				AnimInstance->Montage_SetEndDelegate(MontageEndedDelegate, MontageToPlay);

				ACharacter* Character = Cast<ACharacter>(GetAvatarActor());
				if (Character && (Character->GetLocalRole() == ROLE_Authority ||
					(Character->GetLocalRole() == ROLE_AutonomousProxy && Ability->GetNetExecutionPolicy() == EGameplayAbilityNetExecutionPolicy::LocalPredicted)))
				{
					Character->SetAnimRootMotionTranslationScale(AnimRootMotionTranslationScale);
				}

				bPlayedMontage = true;
			}
		}
		else
		{
			CLog::Print("UAT_MontageNotifyEvent::Activate call to PlayMontage failed!");
		}
	}
	else
	{
		CLog::Print("UAT_MontageNotifyEvent::Activate called on invalid AbilitySystemComponent");
	}

	if (!bPlayedMontage)
	{
		UE_LOG(LogTemp, Warning, TEXT("UAT_MontageNotifyEvent::Activate called in Ability %s failed to play montage %s; Task Instance Name %s."), *Ability->GetName(), *GetNameSafe(MontageToPlay), *InstanceName.ToString());
		if (ShouldBroadcastAbilityTaskDelegates())
		{
			//ABILITY_LOG(Display, TEXT("%s: OnCancelled"), *GetName());
			OnCancelled.Broadcast(FGameplayTag(), FGameplayEventData());
		}
	}

	SetWaitingOnAvatar();
}

void UAT_MontageNotifyEvent::ExternalCancel()
{
	check(AbilitySystemComponent);

	OnAbilityCancelled();

	Super::ExternalCancel();
}

void UAT_MontageNotifyEvent::OnDestroy(bool AbilityEnded)
{
	// 참고: 이 몽타주 종료 델리게이트는 멀티캐스트가 아니기 때문에 굳이 초기화할 필요가 없다.
	// 다음 몽타주가 재생될 때 자동으로 초기화된다.
	// (만약 이 오브젝트가 파괴된다면, 이를 감지하고 아무 작업도 하지 않는다)

	// 하지만 아래의 델리게이트는 멀티캐스트이므로 명시적으로 초기화해줘야 한다
	if (Ability)
	{
		Ability->OnGameplayAbilityCancelled.Remove(CancelledHandle);
		if (AbilityEnded && bStopWhenAbilityEnds)
			StopPlayingMontage();
	}

	UAbilityComponent* skillComp = GetTargetAbilityComp();
	if (skillComp)skillComp->RemoveGameplayEventTagContainerDelegate(EventTags, EventHandle);

	Super::OnDestroy(AbilityEnded);

}

UAbilityComponent* UAT_MontageNotifyEvent::GetTargetAbilityComp()
{
	return Cast<UAbilityComponent>(AbilitySystemComponent);
}

void UAT_MontageNotifyEvent::OnMontageBlendingOut(UAnimMontage* Montage, bool bInterrupted)
{
	if (Ability && Ability->GetCurrentMontage() == MontageToPlay)
	{
		if (Montage == MontageToPlay)
		{
			AbilitySystemComponent->ClearAnimatingAbility(Ability);

			// AnimRootMotionTranslationScale 값을 초기화한다
			ACharacter* Character = Cast<ACharacter>(GetAvatarActor());
			if (Character && (Character->GetLocalRole() == ROLE_Authority ||
				(Character->GetLocalRole() == ROLE_AutonomousProxy && Ability->GetNetExecutionPolicy() == EGameplayAbilityNetExecutionPolicy::LocalPredicted)))
			{
				Character->SetAnimRootMotionTranslationScale(1.f);
			}

		}
	}

	if (bInterrupted)
	{
		if (ShouldBroadcastAbilityTaskDelegates())
		{
			OnInterrupted.Broadcast(FGameplayTag(), FGameplayEventData());
		}
	}
	else
	{
		if (ShouldBroadcastAbilityTaskDelegates())
		{
			OnBlendOut.Broadcast(FGameplayTag(), FGameplayEventData());
		}
	}
}

void UAT_MontageNotifyEvent::OnAbilityCancelled()
{
	// TODO: 이 수정사항을 엔진에 다시 병합할 것. 잘못된 콜백을 호출하고 있었다

	if (StopPlayingMontage())
	{
		// 블루프린트에서도 인터럽트를 처리하도록 한다
		if (ShouldBroadcastAbilityTaskDelegates())
		{
			OnCancelled.Broadcast(FGameplayTag(), FGameplayEventData());
		}
	}
}

void UAT_MontageNotifyEvent::OnMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
	if (!bInterrupted)
	{
		if (ShouldBroadcastAbilityTaskDelegates())
		{
			OnCompleted.Broadcast(FGameplayTag(), FGameplayEventData());
		}
	}

	EndTask();
}

void UAT_MontageNotifyEvent::OnGameplayEvent(FGameplayTag EventTag, const FGameplayEventData* Payload)
{
	// TODO: 쿨다운이 실패를 반환할 경우 처리
	//if (ShouldBroadcastAbilityTaskDelegates())
	{
		FGameplayEventData TempData = *Payload;
		TempData.EventTag = EventTag;

		EventReceived.Broadcast(EventTag, TempData);
	}
}

bool UAT_MontageNotifyEvent::StopPlayingMontage()
{
	const FGameplayAbilityActorInfo* ActorInfo = Ability->GetCurrentActorInfo();
	if (!ActorInfo)return false;

	UAnimInstance* AnimInstance = ActorInfo->GetAnimInstance();
	if (!AnimInstance)return false;

	// 몽타주가 아직 재생 중인지 확인
	// 어빌리티가 인터럽트되었을 경우, 몽타주를 자동으로 중지시켜야 한다
	if (AbilitySystemComponent && Ability)
	{
		if (AbilitySystemComponent->GetAnimatingAbility() == Ability
			&& AbilitySystemComponent->GetCurrentMontage() == MontageToPlay)
		{
			// 델리게이트를 언바인드하여 호출되지 않도록 한다
			FAnimMontageInstance* MontageInstance = AnimInstance->GetActiveInstanceForMontage(MontageToPlay);
			if (MontageInstance)
			{
				MontageInstance->OnMontageBlendingOutStarted.Unbind();
				MontageInstance->OnMontageEnded.Unbind();
			}

			AbilitySystemComponent->CurrentMontageStop();
			return true;
		}
	}

	return false;
}