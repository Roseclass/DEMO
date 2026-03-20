#pragma once

#include "CoreMinimal.h"
#include "GameAbilities/GA_MontageWithEvent.h"
#include "GAPresets.generated.h"

/**
 * 
 */
UCLASS()
class DEMO_API UGA_Hit : public UGA_MontageWithEvent
{
	GENERATED_BODY()
public:
	UGA_Hit()
	{
		AbilityTags.AddTag(FGameplayTag::RequestGameplayTag("Skill.System.Hit"));

		ActivationOwnedTags.AddTag(FGameplayTag::RequestGameplayTag("State.Hit"));

		ActivationBlockedTags.AddTag(FGameplayTag::RequestGameplayTag("State.Dead"));
		ActivationBlockedTags.AddTag(FGameplayTag::RequestGameplayTag("State.Hit"));
		ActivationBlockedTags.AddTag(FGameplayTag::RequestGameplayTag("State.Stun"));

	};
};

UCLASS()
class DEMO_API UGA_StunStart : public UGA_MontageWithEvent
{
	GENERATED_BODY()
public:
	UGA_StunStart()
	{
		AbilityTags.AddTag(FGameplayTag::RequestGameplayTag("Skill.System.StunStart"));

		CancelAbilitiesWithTag.AddTag(FGameplayTag::RequestGameplayTag("Skill.System.Hit"));

		BlockAbilitiesWithTag.AddTag(FGameplayTag::RequestGameplayTag("Skill.System.Hit"));
		BlockAbilitiesWithTag.AddTag(FGameplayTag::RequestGameplayTag("Skill.System.StunStart"));

		ActivationOwnedTags.AddTag(FGameplayTag::RequestGameplayTag("State.Stun"));

		ActivationBlockedTags.AddTag(FGameplayTag::RequestGameplayTag("State.Dead"));
		ActivationBlockedTags.AddTag(FGameplayTag::RequestGameplayTag("State.Stun"));

		FAbilityTriggerData triggerData;
		triggerData.TriggerTag = FGameplayTag::RequestGameplayTag("Effect.CC.Stun");
		triggerData.TriggerSource = EGameplayAbilityTriggerSource::OwnedTagAdded;
		AbilityTriggers.Add(triggerData);
	};
};

UCLASS()
class DEMO_API UGA_StunEnd : public UGA_MontageWithEvent
{
	GENERATED_BODY()
public:
	UGA_StunEnd()
	{
		AbilityTags.AddTag(FGameplayTag::RequestGameplayTag("Skill.System.StunEnd"));

		CancelAbilitiesWithTag.AddTag(FGameplayTag::RequestGameplayTag("Skill.System.Hit"));

		BlockAbilitiesWithTag.AddTag(FGameplayTag::RequestGameplayTag("Skill.System.Hit"));
		BlockAbilitiesWithTag.AddTag(FGameplayTag::RequestGameplayTag("Skill.System.StunEnd"));

		ActivationBlockedTags.AddTag(FGameplayTag::RequestGameplayTag("State.Dead"));
	};
};
