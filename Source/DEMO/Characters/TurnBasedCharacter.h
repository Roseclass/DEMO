#pragma once

#include "CoreMinimal.h"
#include "Characters/BaseCharacter.h"
#include "Characters/TurnBasedCharacterData.h"
#include "TurnBasedCharacter.generated.h"

/**
 * 
 */

class UTurnBasedCameraComponent;

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
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
		USceneComponent* SelectTargetPoint;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
		USceneComponent* SelectSkillPoint;

	//actor
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
		UTurnBasedCameraComponent* TurnBasedCamera;

public:

	//function
private:
protected:
public:
	virtual void Init(FGuid NewSaveName, UPrimaryDataAsset* DA) override;
	virtual FGameplayTag GetDataTag() const override;

	FTransform GetSelectTargetTransform() const;
	FTransform GetSelectSkillTransform() const;
	float GetSpeed() const;
	float GetTurnGauge() const;
};
