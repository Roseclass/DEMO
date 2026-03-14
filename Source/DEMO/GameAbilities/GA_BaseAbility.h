#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "GameAbilities/AbilityEnums.h"
#include "GA_BaseAbility.generated.h"

/**
 * 
 */

UCLASS()
class DEMO_API UGA_BaseAbility : public UGameplayAbility
{
	GENERATED_BODY()
public:
	UGA_BaseAbility();
protected:
public:
	virtual void OnAvatarSet(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec) override;

//property
private:
protected:
public:
	//UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Ability")
	//	EAbilityType AbilityType;

//function
private:
protected:
public:
};
