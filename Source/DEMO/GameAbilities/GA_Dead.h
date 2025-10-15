#pragma once

#include "CoreMinimal.h"
#include "GameAbilities/GA_MontageWithEvent.h"
#include "GA_Dead.generated.h"

/**
 * 
 */

UCLASS()
class DEMO_API UGA_Dead : public UGA_MontageWithEvent
{
	GENERATED_BODY()
public:
	UGA_Dead();
protected:
public:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

	//property
private:
protected:
public:

	//function
private:
protected:
	virtual void EventReceived(FGameplayTag EventTag, FGameplayEventData EventData)override;

public:
};
