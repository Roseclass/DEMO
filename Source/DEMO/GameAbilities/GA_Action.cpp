#include "GameAbilities/GA_Action.h"
#include "Global.h"
#include "GameplayTagContainer.h"

#include "Characters/TPSCharacter.h"
#include "Characters/TPSCharacterData.h"

#include "GameAbilities/AT_MontageNotifyEvent.h"

UGA_Action::UGA_Action()
{

}

void UGA_Action::OnGiveAbility(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec)
{
	Super::OnGiveAbility(ActorInfo, Spec);
	
	UTPSCharacterData* data = Cast<UTPSCharacterData>(Spec.SourceObject);
	CheckTrue_Print(!data, "TPSCharacterData cast Failed!!");

	ActionTag = data->RuntimeData.ActionTag;

	{
		ActionEndTag = FGameplayTag::RequestGameplayTag(
			FName(AppendLeaf(
				ActionTag,
				data->RuntimeData.ActionEndLeaf.ToString()
			)));
		if(!ActionEndTag.IsValid())CLog::Print("UGA_Action::OnGiveAbility ActionEndTag Error");
	}

	{
		ActionOnCollisionTag = FGameplayTag::RequestGameplayTag(
			FName(AppendLeaf(
				ActionTag,
				data->RuntimeData.ActionOnCollisionLeaf.ToString()
			)));
		if (!ActionOnCollisionTag.IsValid())CLog::Print("UGA_Action::OnGiveAbility ActionOnCollisionTag Error");
	}

	{
		ActionOffCollisionTag = FGameplayTag::RequestGameplayTag(
			FName(AppendLeaf(
				ActionTag,
				data->RuntimeData.ActionOffCollisionLeaf.ToString()
			)));
		if (!ActionOffCollisionTag.IsValid())CLog::Print("UGA_Action::OnGiveAbility ActionOffCollisionLeaf Error");
	}

	{
		ActionOnBeginOverlapTag = FGameplayTag::RequestGameplayTag(
			FName(AppendLeaf(
				ActionTag,
				data->RuntimeData.ActionOnBeginOverlapLeaf.ToString()
			)));
		if (!ActionOnBeginOverlapTag.IsValid())CLog::Print("UGA_Action::OnGiveAbility ActionOnBeginOverlapLeaf Error");
	}

	{
		ActionOnEndOverlapTag = FGameplayTag::RequestGameplayTag(
			FName(AppendLeaf(
				ActionTag,
				data->RuntimeData.ActionOnEndOverlapLeaf.ToString()
			)));
		if (!ActionOnEndOverlapTag.IsValid())CLog::Print("UGA_Action::OnGiveAbility ActionOnEndOverlapLeaf Error");
	}

}

FString UGA_Action::AppendLeaf(const FGameplayTag& Route, FString Leaf)
{
	return Route.ToString() + TEXT(".") + Leaf;
}

void UGA_Action::EventReceived(FGameplayTag EventTag, FGameplayEventData EventData)
{
	if(EventTag == ActionEndTag)
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);

	ATPSCharacter* owner = Cast<ATPSCharacter>(GetOwningActorFromActorInfo());
	CheckTrue_Print(!owner, "owner cast failed");

	if (EventTag == ActionOnCollisionTag)
	{
		CLog::Print("ActionOnCollisionTag");
		owner->EnableTriggerCollision(1);
	}
	else if (EventTag == ActionOffCollisionTag)
	{
		CLog::Print("ActionOffCollisionTag");
		owner->EnableTriggerCollision(0);
	}
	else if (EventTag == ActionOnBeginOverlapTag)
	{
		CLog::Print("ActionOnBeginOverlapTag");
	}
	else if (EventTag == ActionOnEndOverlapTag)
	{
		CLog::Print("ActionOnEndOverlapTag");
	}
}
