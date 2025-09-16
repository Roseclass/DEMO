#include "Objects/TurnbasedPhaseCamera.h"
#include "Global.h"

#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"

#include "Characters/TurnBasedCharacter.h"
#include "Characters/TurnBasedCharacterData.h"
#include "Datas/TurnBasedDataTypes.h"

ATurnbasedPhaseCamera::ATurnbasedPhaseCamera()
{
	PrimaryActorTick.bCanEverTick = true;
	CHelpers::CreateComponent<USpringArmComponent>(this, &CameraBoom, "CameraBoom");
	CHelpers::CreateComponent<UCameraComponent>(this, &FollowCamera, "FollowCamera", CameraBoom);
	CameraBoom->bEnableCameraLag = 1;
}

void ATurnbasedPhaseCamera::BeginPlay()
{
	Super::BeginPlay();	
}

void ATurnbasedPhaseCamera::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bRotating)RInterpToTarget(DeltaTime);
}

void ATurnbasedPhaseCamera::RInterpToTarget(float DeltaTime)
{
	FRotator current = GetActorRotation();
	current = UKismetMathLibrary::RInterpTo(current, TargetRotation, DeltaTime, RotatingSpeed);
	SetActorRotation(current);
	if (current.Equals(TargetRotation,1))
		bRotating = 0;
}

void ATurnbasedPhaseCamera::Init(FTransform InTransform)
{
	InitialTransform = InTransform;
}

void ATurnbasedPhaseCamera::FocusSelectTarget()
{
	SetActorTransform(InitialTransform);
}

void ATurnbasedPhaseCamera::FocusSelectSkill(ATurnBasedCharacter* InCurrentTurnCharacter)
{

}

void ATurnbasedPhaseCamera::SetTargetRotation(FRotator NewRotation)
{
	TargetRotation = NewRotation;
	bRotating = 1;
}

bool ATurnbasedPhaseCamera::IsRotating()const 
{
	CLog::Print(bRotating);
	return bRotating; 
}
