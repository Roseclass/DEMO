#include "GameAbilities/GA_Dead.h"
#include "Global.h"

#include "GameplayCueManager.h"
#include "GameplayTagContainer.h"
#include "AbilitySystemGlobals.h"

#include "Characters/TurnBasedCharacter.h"
#include "GameAbilities/AbilityComponent.h"
#include "Objects/DamageDealer.h"

UGA_Dead::UGA_Dead()
{
	AbilityTags.AddTag(FGameplayTag::RequestGameplayTag("Skill.System.End"));
}

void UGA_Dead::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
}

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
