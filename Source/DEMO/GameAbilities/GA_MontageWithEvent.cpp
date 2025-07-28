#include "GameAbilities/GA_MontageWithEvent.h"
#include "Global.h"
#include "GameplayTagContainer.h"

#include "GameAbilities/AT_MontageNotifyEvent.h"

UGA_MontageWithEvent::UGA_MontageWithEvent()
{

}

void UGA_MontageWithEvent::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	// Play montage and wait for event
	UAT_MontageNotifyEvent* Task = UAT_MontageNotifyEvent::CreateMontageNotifyEvent(this, NAME_None, Montage, FGameplayTagContainer(), 1.0f, NAME_None, false, 1.0f);
	Task->OnBlendOut.AddDynamic(this, &UGA_MontageWithEvent::OnCompleted);
	Task->OnCompleted.AddDynamic(this, &UGA_MontageWithEvent::OnCompleted);
	Task->OnInterrupted.AddDynamic(this, &UGA_MontageWithEvent::OnCancelled);
	Task->OnCancelled.AddDynamic(this, &UGA_MontageWithEvent::OnCancelled);
	Task->EventReceived.AddDynamic(this, &UGA_MontageWithEvent::EventReceived);

	// ReadyForActivation() is how you activate the AbilityTask in C++. Blueprint has magic from K2Node_LatentGameplayTaskCall that will automatically call ReadyForActivation().
	Task->ReadyForActivation();
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
}