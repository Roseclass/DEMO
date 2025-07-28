// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameAbilities/GA_BaseAbility.h"
#include "GA_MontageWithEvent.generated.h"

/**
 * 
 */

UCLASS()
class DEMO_API UGA_MontageWithEvent : public UGA_BaseAbility
{
	GENERATED_BODY()
public:
	UGA_MontageWithEvent();
protected:
public:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

	//property
private:
protected:
	UPROPERTY(EditAnywhere, Category = "Montage")
		UAnimMontage* Montage;

	UPROPERTY(EditAnywhere, Category = "Montage")
		float PlayRate = 1.0f;

	UPROPERTY(EditAnywhere, Category = "Montage")
		FName StartSection;

	UPROPERTY(EditAnywhere, Category = "Montage")
		FName RepeatSection;
public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Tags, meta = (Categories = "AbilityTagCategory"))
		FGameplayTag EndTag;

	//function
private:
protected:
	UFUNCTION()virtual void OnCancelled(FGameplayTag EventTag, FGameplayEventData EventData);
	UFUNCTION()virtual void OnCompleted(FGameplayTag EventTag, FGameplayEventData EventData);
	UFUNCTION()virtual void EventReceived(FGameplayTag EventTag, FGameplayEventData EventData);
public:
};
