#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "GameplayTagContainer.h"
#include "TurnBasedAnimBlueprint.generated.h"

/**
 * 
 */

class ATurnBasedCharacter;

UCLASS()
class DEMO_API UTurnBasedAnimBlueprint : public UAnimInstance
{
	GENERATED_BODY()
public:
protected:
public:
	virtual void NativeInitializeAnimation() override;
	virtual void NativeBeginPlay() override;
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;

	//property
private:
	ATurnBasedCharacter* OwnerCharacter;
protected:
	UPROPERTY(BlueprintReadOnly)
		FGameplayTag CurrentSkillTag;

#if WITH_EDITORONLY_DATA
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Preview")
		FGameplayTag PreviewSkillTag;
#endif

public:

	//function
private:
	UFUNCTION()void ChangeSkillTag(FGameplayTag NewSkillTag);
protected:
public:
};