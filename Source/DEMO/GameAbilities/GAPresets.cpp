#include "GameAbilities/GAPresets.h"
#include "Global.h"
#include "GameAbilities/AbilityComponent.h"

void UGA_Dead::EventReceived(FGameplayTag EventTag, FGameplayEventData EventData)
{
	UAbilityComponent* asc = Cast<UAbilityComponent>(GetCurrentActorInfo()->AbilitySystemComponent);

	if (EventTag == FGameplayTag::RequestGameplayTag("Skill.System.Dead"))
	{
		//EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
		asc->BroadcastOnDeadSequenceEnd();
		return;
	}
}

