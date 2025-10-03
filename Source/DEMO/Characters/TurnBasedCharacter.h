#pragma once

#include "CoreMinimal.h"
#include "Characters/BaseCharacter.h"
#include "Characters/TurnBasedCharacterData.h"
#include "TurnBasedCharacter.generated.h"

/**
 * 
 */

class UTurnBasedCameraComponent;

DECLARE_MULTICAST_DELEGATE_OneParam(FOnAnimTagChanged, FGameplayTag);

UCLASS()
class DEMO_API ATurnBasedCharacter : public ABaseCharacter
{
	GENERATED_BODY()
public:
	ATurnBasedCharacter();
protected:
	virtual void BeginPlay() override;
public:
	virtual void Tick(float DeltaTime) override;

	//property
private:
	FTurnBasedCharacterRuntimeData RuntimeData;
protected:
	//scene

	//actor
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
		UTurnBasedCameraComponent* TurnBasedCamera;

public:
	FOnAnimTagChanged OnAnimTagChanged;

	//function
private:
protected:
public:
	virtual void Init(FGuid NewSaveName, UPrimaryDataAsset* DA) override;
	virtual FGameplayTag GetDataTag() const override;

	//from runtime
	FTransform GetSelectTargetTransform() const;
	FTransform GetSelectSkillTransform() const;
	FTransform GetSelectSkillRelativeTransform() const;
	TArray<FGameplayTag> GetEquippedSkillTags() const;

	//from asc
	float GetSpeed() const;
	float GetTurnGauge() const;
};
