#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TurnbasedPhaseCamera.generated.h"

class USpringArmComponent;
class UCameraComponent;
class ATurnBasedCharacter;

UCLASS()
class DEMO_API ATurnbasedPhaseCamera : public AActor
{
	GENERATED_BODY()
	
public:	
	ATurnbasedPhaseCamera();
protected:
	virtual void BeginPlay() override;
public:	
	virtual void Tick(float DeltaTime) override;

	//property
private:
	FTransform InitialTransform;
	bool bRotating;
	float RotatingSpeed = 40;
	FRotator TargetRotation;
protected:
	//scene
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
		USpringArmComponent* CameraBoom;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
		UCameraComponent* FollowCamera;

	//actor
public:

	//function
private:
	void RInterpToTarget(float DeltaTime);
protected:
public:
	void Init(FTransform InTransform);
	void FocusSelectTarget();
	void FocusSelectSkill(ATurnBasedCharacter* InCurrentTurnCharacter);
	void SetTargetRotation(FRotator NewRotation);
	bool IsRotating()const;
};
