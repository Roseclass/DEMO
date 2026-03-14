#include "GameAbilities/AT_SkillNotifyEvent.h"
#include "Global.h"
#include "AbilitySystemGlobals.h"
#include "Animation/AnimInstance.h"
#include "GameFramework/Character.h"

#include "GameAbilities/AbilityComponent.h"

UAT_SkillNotifyEvent::UAT_SkillNotifyEvent()
{

}

UAT_SkillNotifyEvent* UAT_SkillNotifyEvent::CreateSkillNotifyEvent(UGameplayAbility* OwningAbility, FName TaskInstanceName, FGameplayTagContainer EventTags)
{
	UAT_SkillNotifyEvent* result = NewAbilityTask<UAT_SkillNotifyEvent>(OwningAbility, TaskInstanceName);
	result->EventTags = EventTags;

	return result;
}

void UAT_SkillNotifyEvent::Activate()
{
	CheckNull(Ability);

	UAbilityComponent* asc = GetTargetAbilityComp();

	if (asc)
	{
		// 이벤트 콜백에 바인딩한다
		EventHandle = asc->AddGameplayEventTagContainerDelegate(EventTags, FGameplayEventTagMulticastDelegate::FDelegate::CreateUObject(this, &UAT_SkillNotifyEvent::OnGameplayEvent));
		EndHandle = asc->OnSkillEnd.AddLambda([this]()
			{
				this->EndTask();
			});
	}
	else
	{
		CLog::Print("UAT_SkillNotifyEvent::Activate called on invalid AbilitySystemComponent");
	}

	SetWaitingOnAvatar();
}

void UAT_SkillNotifyEvent::OnDestroy(bool AbilityEnded)
{
	UAbilityComponent* asc = GetTargetAbilityComp();
	if (asc)
	{
		asc->RemoveGameplayEventTagContainerDelegate(EventTags, EventHandle);
		asc->OnSkillEnd.Remove(EndHandle);
	}

	Super::OnDestroy(AbilityEnded);
}

UAbilityComponent* UAT_SkillNotifyEvent::GetTargetAbilityComp()
{
	return Cast<UAbilityComponent>(AbilitySystemComponent);
}

void UAT_SkillNotifyEvent::OnGameplayEvent(FGameplayTag EventTag, const FGameplayEventData* Payload)
{
	{
		FGameplayEventData TempData = *Payload;
		TempData.EventTag = EventTag;

		EventReceived.Broadcast(EventTag, TempData);
	}
}
