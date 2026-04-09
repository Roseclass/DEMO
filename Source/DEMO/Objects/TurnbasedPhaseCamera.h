#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Engine/DataTable.h"
#include "GameAbilities/GameplayEffectContexts.h"
#include "TurnbasedPhaseCamera.generated.h"

class USpringArmComponent;
class UCameraComponent;
class ATurnBasedCharacter;

USTRUCT(BlueprintType)
struct FCameraPreset : public FTableRowBase
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere)ECameraShotType ShotType;
	UPROPERTY(EditAnywhere)FVector BaseOffset;
	UPROPERTY(EditAnywhere)float BaseArmLength;
	UPROPERTY(EditAnywhere)float YawRange;
	UPROPERTY(EditAnywhere)float PitchRange;
	UPROPERTY(EditAnywhere)float ZOffsetRange;
	UPROPERTY(EditAnywhere)float BlendTime;
};

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
	bool bLocating;
	bool bRotating;

	AActor* OriginActor;
	AActor* DestActor;
	FMoveCameraData CurrentData;
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
	void BlendToTarget(float DeltaTime);
	void HandleShotType(const FMoveCameraContext* InEffectContext);
	FVector GetBlendOrigin();
	FVector GetBlendDest();
	float GetSpringArmLength();
protected:
public:
	void Init(FTransform InTransform);
	void FocusAvailableTargets(TArray<ATurnBasedCharacter*> AvailableTargets);
	void FocusSelectSkill(ATurnBasedCharacter* InCurrentTurnCharacter);
	void SetTargetRotation(FRotator NewRotation);
	bool IsRotating()const;

	void ApplyCameraMove(const FMoveCameraContext* InEffectContext);
};
