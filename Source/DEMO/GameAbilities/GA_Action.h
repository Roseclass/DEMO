#pragma once

#include "CoreMinimal.h"
#include "GameAbilities/GA_MontageWithEvent.h"
#include "GA_Action.generated.h"

/**
 * 
 */

UCLASS()
class DEMO_API UGA_Action : public UGA_MontageWithEvent
{
	GENERATED_BODY()
public:
	UGA_Action();
public:
	virtual void OnGiveAbility(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec) override;

	//property
private:
	FGameplayTag ActionTag;
	FGameplayTag ActionEndTag;
	FGameplayTag ActionOnCollisionTag;
	FGameplayTag ActionOffCollisionTag;
	FGameplayTag ActionOnBeginOverlapTag;
	FGameplayTag ActionOnEndOverlapTag;
protected:
public:

	//function
private:
	FString AppendLeaf(const FGameplayTag& Route, FString Leaf);
protected:
	virtual void EventReceived(FGameplayTag EventTag, FGameplayEventData EventData);
public:

};
