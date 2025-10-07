#include "Objects/TurnbasedPhaseCamera.h"
#include "Global.h"

#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"

#include "Characters/TurnBasedCharacter.h"
#include "Characters/TurnBasedCharacterData.h"
#include "Datas/TurnBasedDataTypes.h"
#include "GameAbilities/AbilityComponent.h"

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

	if (bRotating)
	{
		RInterpToGoal(DeltaTime);
	}
	else if (bReturning)
	{
		ReturnInterp(DeltaTime);
	}
}

void ATurnbasedPhaseCamera::ReturnInterp(float DeltaTime)
{
	FTransform current = GetActorTransform();
	FTransform goal = CurrentMoveEffect.CameraReturnType == ECameraReturnType::Prev ? ReturnTransform : InitialTransform;
	current = UKismetMathLibrary::TInterpTo(
		current,
		goal,
		DeltaTime,
		4.6 / BlendTime);
	SetActorTransform(current);
	if (current.Equals(goal, 1))
		bReturning = 0;
}

void ATurnbasedPhaseCamera::RInterpToGoal(float DeltaTime)
{
	ElapsedTime += DeltaTime;

	if (LookAtTargetActor.IsValid())
	{
		SetActorRotation(
			FQuat4d(
				UKismetMathLibrary::FindLookAtRotation
				(GetActorLocation(), LookAtTargetActor->GetActorLocation() + CurrentMoveEffect.LookAtOffset)
			));
	}
	else
	{
		SetActorTransform(
		UKismetMathLibrary::TInterpTo(
			GetActorTransform(),
			GoalTrasnform,
			DeltaTime,
			4.6 / CurrentMoveEffect.Duration));
	}

	if (CurrentMoveEffect.Duration <= ElapsedTime)
	{
		ElapsedTime = 0;
		FDetachmentTransformRules detachrules(EDetachmentRule::KeepWorld, 1);
		DetachFromActor(detachrules);

		LocationActor = nullptr;
		LookAtTargetActor = nullptr;

		bRotating = 0;
		if (!CurrentMoveEffect.bAutoReturn)return;
		bReturning = 1;
	}
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
	CurrentTurnCharacter = InCurrentTurnCharacter;
	SetActorTransform(InCurrentTurnCharacter->GetSelectSkillTransform());
}

void ATurnbasedPhaseCamera::SetTargetRotation(FRotator NewRotation)
{
	SetActorRotation(NewRotation);
	//GoalTrasnform.SetRotation(FQuat4d(NewRotation));
}

bool ATurnbasedPhaseCamera::IsRotating()const 
{
	return bRotating || bReturning;
}

void ATurnbasedPhaseCamera::ApplyCameraMove(const FCameraMoveEffectContext* InEffectContext)
{
	CurrentMoveEffect = *InEffectContext;

	ReturnTransform = GetActorTransform();
	CameraBoom->TargetArmLength = CurrentMoveEffect.ArmLength;
	CameraBoom->bDoCollisionTest = CurrentMoveEffect.bEnableArmCollision;

	if (CurrentMoveEffect.CameraMoveType == ECameraMoveType::SplineLocation)
	{
		//TODO
	}
	else if (CurrentMoveEffect.CameraMoveType == ECameraMoveType::FixedLocation)
	{
		SetActorLocation(CurrentMoveEffect.Location + CurrentMoveEffect.LocationOffset);
		GoalTrasnform.SetTranslation(CurrentMoveEffect.Location + CurrentMoveEffect.LocationOffset);
	}
	else if (CurrentMoveEffect.CameraMoveType == ECameraMoveType::CurrentToGoal)
	{
		GoalTrasnform.SetTranslation(CurrentMoveEffect.Location + CurrentMoveEffect.LocationOffset);
	}
	else if (CurrentMoveEffect.CameraMoveType == ECameraMoveType::StartToGoal)
	{
		SetActorLocation(CurrentMoveEffect.StartLocation + CurrentMoveEffect.LocationOffset);
		GoalTrasnform.SetTranslation(CurrentMoveEffect.Location + CurrentMoveEffect.LocationOffset);
	}
	else if (CurrentMoveEffect.CameraMoveType == ECameraMoveType::Target)
	{
		if (CurrentMoveEffect.LocationTargetActorType == ECameraGoalActorType::Owner) LocationActor = CurrentTurnCharacter;
		else if (CurrentMoveEffect.LocationTargetActorType == ECameraGoalActorType::Target)
		{
			UAbilityComponent* asc = Cast<UAbilityComponent>(CurrentTurnCharacter->GetAbilitySystemComponent());
			LocationActor = asc->GetTarget();
		}
		FAttachmentTransformRules rules(EAttachmentRule::KeepWorld, true);
		AttachToActor(LocationActor.Get(), rules);
		SetActorRelativeLocation(CurrentMoveEffect.LocationOffset);
	}

	if (CurrentMoveEffect.CameraLookAtType == ECameraLookAtType::SplineLocation)
	{
		//TODO
	}
	else if (CurrentMoveEffect.CameraLookAtType == ECameraLookAtType::FixedLocation)
	{
		SetActorRotation(
			FQuat4d(
				UKismetMathLibrary::FindLookAtRotation
				(GetActorLocation(), CurrentMoveEffect.LookAtLocation + CurrentMoveEffect.LookAtOffset)
			));
		GoalTrasnform.SetRotation(
			FQuat4d(
				UKismetMathLibrary::FindLookAtRotation
				(GetActorLocation(), CurrentMoveEffect.LookAtLocation + CurrentMoveEffect.LookAtOffset)
			));
	}
	else if (CurrentMoveEffect.CameraLookAtType == ECameraLookAtType::CurrentToGoal)
	{
		GoalTrasnform.SetRotation(
			FQuat4d(
				UKismetMathLibrary::FindLookAtRotation
				(CurrentMoveEffect.Location, CurrentMoveEffect.LookAtLocation + CurrentMoveEffect.LookAtOffset)
			));
	}
	else if (CurrentMoveEffect.CameraLookAtType == ECameraLookAtType::StartToGoal)
	{
		SetActorRotation(
			FQuat4d(
				UKismetMathLibrary::FindLookAtRotation
				(GetActorLocation(), InEffectContext->StartLookAtLocation + InEffectContext->LookAtOffset)
			));
		GoalTrasnform.SetRotation(
			FQuat4d(
				UKismetMathLibrary::FindLookAtRotation
				(CurrentMoveEffect.Location, CurrentMoveEffect.LookAtLocation + CurrentMoveEffect.LookAtOffset)
			));
	}
	else if (CurrentMoveEffect.CameraLookAtType == ECameraLookAtType::Target)
	{
		if (CurrentMoveEffect.LookAtTargetActorType == ECameraGoalActorType::Owner) LookAtTargetActor = CurrentTurnCharacter;
		else if (CurrentMoveEffect.LookAtTargetActorType == ECameraGoalActorType::Target)
		{
			UAbilityComponent* asc = Cast<UAbilityComponent>(CurrentTurnCharacter->GetAbilitySystemComponent());
			LookAtTargetActor = asc->GetTarget();
		}
	}

	bRotating = 1;
	bReturning = 0;
}