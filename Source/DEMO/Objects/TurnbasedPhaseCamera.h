#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Engine/DataTable.h"
#include "GameAbilities/GameplayEffectPayloads.h"
#include "TurnbasedPhaseCamera.generated.h"

class USpringArmComponent;
class UCameraComponent;
class ATurnBasedCharacter;
class UDA_MoveCamera;

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
	UDataTable* CameraPresetDT;
	TMap<ECameraShotType, FCameraPreset*>CameraPresetDatas;

	TWeakObjectPtr<ATurnBasedCharacter> CurrentTurnCharacter;

	FTransform InitialTransform;
	FVector GoalLocation;
	FVector LookAtLocation;
	bool bRotating;
	bool bReturning;
	float BlendTime;
	float ElapsedTime;

	AActor* ShotOrigin;
	UDA_MoveCamera* CurrentDA;
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
	void InterpToGoal(float DeltaTime);
	void HandleShotType();
	void HandleEventType();
	void HandleTargetCount();
	void HandleSkillType();	
protected:
public:
	void Init(FTransform InTransform);
	void FocusSelectTarget();
	void FocusSelectSkill(ATurnBasedCharacter* InCurrentTurnCharacter);
	void SetTargetRotation(FRotator NewRotation);
	bool IsRotating()const;

	void ApplyCameraMove(const FPayloadContext* InEffectContext);
};
