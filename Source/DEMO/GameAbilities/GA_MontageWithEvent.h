#pragma once

#include "CoreMinimal.h"
#include "GameAbilities/GA_BaseAbility.h"
#include "GameAbilities/GameplayEffectContexts.h"
#include "GA_MontageWithEvent.generated.h"

/**
 * 
 */

USTRUCT(BlueprintType)
struct FGameAbilityMontageData
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, Category = "Data", meta = (ClampMin = 0.10))
		float StartDelay;

	UPROPERTY(EditAnywhere, Category = "Data")
		UAnimMontage* KeyMontage;

	UPROPERTY(EditAnywhere, Category = "Data")
		TArray<UAnimMontage*> SubMontages;

	UPROPERTY(EditAnywhere, Category = "Data")
		float PlayRate = 1.0f;

	UPROPERTY(EditAnywhere, Category = "Data")
		FName StartSection;

	UPROPERTY(EditAnywhere, Category = "Data")
		FName RepeatSection;
};

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
	int32 MontageDataIdx;
	UPROPERTY(EditAnywhere, Category = "Data")
		TArray<FGameAbilityMontageData> MontageDatas;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		TObjectPtr<UDA_GCNPayload> EventPayloads;

	int32 MoveCameraDataIdx;
public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Tags, meta = (Categories = "AbilityTagCategory"))
		FGameplayTag NextMontageTriggerTag;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Tags, meta = (Categories = "AbilityTagCategory"))
		FGameplayTag NextCameraMoveTriggerTag;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Tags, meta = (Categories = "AbilityTagCategory"))
		FGameplayTag EndTag;

	//function
private:
protected:
	virtual void InitAbility();
	virtual void PlayKeyMontage();
	virtual void PlaySubMontages();
	virtual void MoveCamera();

	UFUNCTION()virtual void OnCancelled(FGameplayTag EventTag, FGameplayEventData EventData);
	UFUNCTION()virtual void OnCompleted(FGameplayTag EventTag, FGameplayEventData EventData);
	UFUNCTION()virtual void EventReceived(FGameplayTag EventTag, FGameplayEventData EventData);
public:
};
