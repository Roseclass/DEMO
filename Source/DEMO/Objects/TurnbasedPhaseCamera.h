#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameAbilities/GameplayEffectContexts.h"
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
	TWeakObjectPtr<ATurnBasedCharacter> CurrentTurnCharacter;

	FTransform InitialTransform;
	FTransform ReturnTransform;
	FTransform GoalTrasnform;
	bool bRotating;
	bool bReturning;
	float BlendTime = 0.1;
	float ElapsedTime;

	FCameraMoveEffectContext CurrentMoveEffect;
	TWeakObjectPtr<AActor> LocationActor;
	TWeakObjectPtr<AActor> LookAtTargetActor;
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
	void ReturnInterp(float DeltaTime);
	void RInterpToGoal(float DeltaTime);
protected:
public:
	void Init(FTransform InTransform);
	void FocusSelectTarget();
	void FocusSelectSkill(ATurnBasedCharacter* InCurrentTurnCharacter);
	void SetTargetRotation(FRotator NewRotation);
	bool IsRotating()const;

	void ApplyCameraMove(const FCameraMoveEffectContext* InEffectContext);
};
