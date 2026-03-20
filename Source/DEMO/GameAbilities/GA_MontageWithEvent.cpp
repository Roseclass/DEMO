#include "GameAbilities/GA_MontageWithEvent.h"
#include "Global.h"
#include "GameplayTagContainer.h"

#include "Characters/TurnBasedCharacter.h"

#include "GameAbilities/AbilityComponent.h"
#include "GameAbilities/AT_MontageNotifyEvent.h"

UGA_MontageWithEvent::UGA_MontageWithEvent()
{
	EndTag = FGameplayTag::RequestGameplayTag("Skill.System.End");
	NextMontageTriggerTag = FGameplayTag::RequestGameplayTag("Skill.System.NextMontage");
	NextPayloadEventTriggerTag = FGameplayTag::RequestGameplayTag("Skill.System.NextPayloadEvent");
}

void UGA_MontageWithEvent::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	InitAbility();
}

void UGA_MontageWithEvent::InitAbility()
{
	MontageDataIdx = 0;
	PayloadEventDataIdx = 0;

	FTimerHandle WaitHandle;
	GetWorld()->GetTimerManager().SetTimer(WaitHandle, FTimerDelegate::CreateLambda([&]()
		{
			UAT_MontageNotifyEvent* Task = UAT_MontageNotifyEvent::CreateMontageNotifyEvent(
				this,
				NAME_None,
				MontageDatas[MontageDataIdx].KeyMontage,
				FGameplayTagContainer(),
				MontageDatas[MontageDataIdx].PlayRate,
				MontageDatas[MontageDataIdx].StartSection,
				false);
			Task->OnBlendOut.AddDynamic(this, &UGA_MontageWithEvent::OnCompleted);
			Task->OnCompleted.AddDynamic(this, &UGA_MontageWithEvent::OnCompleted);
			Task->OnInterrupted.AddDynamic(this, &UGA_MontageWithEvent::OnCancelled);
			Task->OnCancelled.AddDynamic(this, &UGA_MontageWithEvent::OnCancelled);
			Task->EventReceived.AddDynamic(this, &UGA_MontageWithEvent::EventReceived);

			// ReadyForActivation()는 C++에서 AbilityTask를 활성화 시킨다. Blueprint는 K2Node_LatentGameplayTaskCall에서 자동으로 ReadyForActivation()를 호출한다.
			Task->ReadyForActivation();

			PlaySubMontages();

			MontageDataIdx++;

		}), MontageDatas[MontageDataIdx].StartDelay, false);
	ExecutePayloadEvent();
}

void UGA_MontageWithEvent::PlayKeyMontage()
{
	UAbilityComponent* asc = Cast<UAbilityComponent>(GetCurrentActorInfo()->AbilitySystemComponent);
	asc->PlayMontage(this, GetCurrentActivationInfo(), MontageDatas[MontageDataIdx].KeyMontage, MontageDatas[MontageDataIdx].PlayRate, MontageDatas[MontageDataIdx].StartSection);
}

void UGA_MontageWithEvent::PlaySubMontages()
{
	UAbilityComponent* asc = Cast<UAbilityComponent>(GetCurrentActorInfo()->AbilitySystemComponent);
	for(auto montage : MontageDatas[MontageDataIdx].SubMontages)
		asc->PlayMontage(this, GetCurrentActivationInfo(), montage, MontageDatas[MontageDataIdx].PlayRate, MontageDatas[MontageDataIdx].StartSection);
}

void UGA_MontageWithEvent::ExecutePayloadEvent()
{
	CheckTrue(!PayloadEventDatas.IsValidIndex(PayloadEventDataIdx));
	UAbilityComponent* asc = Cast<UAbilityComponent>(GetCurrentActorInfo()->AbilitySystemComponent);
	FGameplayCueParameters gameplayCueParameters;

	FPayloadContext* context = PayloadEventDatas[PayloadEventDataIdx++].Duplicate();
	context->RuleSourceActor = GetCurrentActorInfo()->AvatarActor.Get();
	context->EventCauserActor = GetCurrentActorInfo()->AvatarActor.Get();
	context->EventTargetActor = asc->GetTarget();

	gameplayCueParameters.EffectContext = FGameplayEffectContextHandle(context);
	asc->ExecuteGameplayCue(context->GCNTag, gameplayCueParameters);
}

void UGA_MontageWithEvent::OnCancelled(FGameplayTag EventTag, FGameplayEventData EventData)
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
}

void UGA_MontageWithEvent::OnCompleted(FGameplayTag EventTag, FGameplayEventData EventData)
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
}

void UGA_MontageWithEvent::EventReceived(FGameplayTag EventTag, FGameplayEventData EventData)
{
	if (EventTag == EndTag)
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
	else if (EventTag == NextMontageTriggerTag)
	{
		PlayKeyMontage();
		PlaySubMontages();
		++MontageDataIdx;
		return;
	}
	else if (EventTag == NextPayloadEventTriggerTag)
	{
		ExecutePayloadEvent();
		return;
	}
}