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

	CHelpers::GetAsset<UDataTable>(&CameraPresetDT, "DataTable'/Game/Datas/DT_CameraPreset.DT_CameraPreset'");
}

void ATurnbasedPhaseCamera::BeginPlay()
{
	Super::BeginPlay();	

	TArray<FCameraPreset*> arr;
	CameraPresetDT->GetAllRows("", arr); 
	for (auto data : arr)
		CameraPresetDatas.FindOrAdd(data->ShotType) = data;
}

void ATurnbasedPhaseCamera::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bRotating)
	{
		InterpToGoal(DeltaTime);
	}
	else if (bReturning)
	{
		ReturnInterp(DeltaTime);
	}
}

void ATurnbasedPhaseCamera::ReturnInterp(float DeltaTime)
{
	//FTransform current = GetActorTransform();
	//FTransform goal = CurrentMoveEffect.CameraReturnType == ECameraReturnType::Prev ? ReturnTransform : InitialTransform;
	//current = UKismetMathLibrary::TInterpTo(
	//	current,
	//	goal,
	//	DeltaTime,
	//	4.6 / BlendTime);
	//SetActorTransform(current);
	//if (current.Equals(goal, 1))
	//	bReturning = 0;
}

void ATurnbasedPhaseCamera::InterpToGoal(float DeltaTime)
{
	if (BlendTime < 1e-9)
	{
		SetActorLocation(GoalLocation);
		ElapsedTime = 0;
		bRotating = 0;
		return;
	}

	ElapsedTime += DeltaTime;

	//if (LookAtTargetActor.IsValid())
	//{
	//	SetActorRotation(
	//		FQuat4d(
	//			UKismetMathLibrary::FindLookAtRotation
	//			(GetActorLocation(), LookAtTargetActor->GetActorLocation() + CurrentMoveEffect.LookAtOffset)
	//		));
	//}

	{
		SetActorLocation(
		UKismetMathLibrary::VInterpTo(
			GetActorLocation(),
			GoalLocation,
			DeltaTime,
			4.6 / BlendTime));
	}

	if (BlendTime <= ElapsedTime)
	{
		FDetachmentTransformRules detachrules(EDetachmentRule::KeepWorld, 1);
		DetachFromActor(detachrules);
		ElapsedTime = 0;
		bRotating = 0;
	}
}

void ATurnbasedPhaseCamera::HandleShotType()
{
	FCameraPreset* preset = CameraPresetDatas.FindOrAdd(CurrentMoveEffect.ShotType);
	FVector newLoc = preset->BaseOffset + CurrentMoveEffect.ShotOriginActor->GetActorLocation();
	float armlength = preset->BaseArmLength;

	GoalLocation = newLoc;
	BlendTime = preset->BlendTime;
	CameraBoom->TargetArmLength = armlength;
}

void ATurnbasedPhaseCamera::HandleEventType()
{
	//TurnStart,ActionStart,HitConfirmed,Explosion,Interrupt,Death,ActionEnd
}

void ATurnbasedPhaseCamera::HandleTargetCount()
{
	//single
	if (CurrentMoveEffect.TargetActors.Num() == 2)
	{
		CameraBoom->TargetArmLength = CameraBoom->TargetArmLength * 0.9f;
	}
	//multi
	else if (2 < CurrentMoveEffect.TargetActors.Num())
	{
		CameraBoom->TargetArmLength = CameraBoom->TargetArmLength * 1.2f;
		GoalLocation += FVector(0, 0, 40);
	}
}

void ATurnbasedPhaseCamera::HandleSkillType()
{
	if (CurrentMoveEffect.LookAtType == ECameraLookAtType::Target_Primary)
	{
		LookAtLocation = CurrentMoveEffect.TargetActors[0]->GetActorLocation();
	}
	else if (CurrentMoveEffect.LookAtType == ECameraLookAtType::Target_RandomOnce)
	{
		LookAtLocation = CurrentMoveEffect.TargetActors
			[UKismetMathLibrary::RandomIntegerInRange(0, CurrentMoveEffect.TargetActors.Num() - 1)]
		->GetActorLocation();
	}
	else if (CurrentMoveEffect.LookAtType == ECameraLookAtType::Target_Center)
	{
		for(auto i : CurrentMoveEffect.TargetActors)
			LookAtLocation += i->GetActorLocation();
		LookAtLocation /= CurrentMoveEffect.TargetActors.Num();
	}
	else if (CurrentMoveEffect.LookAtType == ECameraLookAtType::ImpactPoint)
	{
		LookAtLocation = CurrentMoveEffect.GetHitResult()->ImpactPoint;
	}
	else if (CurrentMoveEffect.LookAtType == ECameraLookAtType::Origin_Forward)
	{
		LookAtLocation =
			CurrentMoveEffect.ShotOriginActor->GetActorLocation() +
			CurrentMoveEffect.ShotOriginActor->GetActorForwardVector();
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
	CameraBoom->TargetArmLength = 300.f;
	CameraBoom->bDoCollisionTest = 1;
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
	HandleShotType();
	HandleEventType();
	HandleTargetCount();
	HandleSkillType();
	bRotating = 1;
}

//void ATurnbasedPhaseCamera::ApplyCameraMove(const FCameraMoveEffectContext* InEffectContext)
//{
//	CurrentMoveEffect = *InEffectContext;
//
//	ReturnTransform = GetActorTransform();
//	CameraBoom->TargetArmLength = CurrentMoveEffect.ArmLength;
//	CameraBoom->bDoCollisionTest = CurrentMoveEffect.bEnableArmCollision;
//
//	if (CurrentMoveEffect.CameraMoveType == ECameraMoveType::SplineLocation)
//	{
//		//TODO
//	}
//	else if (CurrentMoveEffect.CameraMoveType == ECameraMoveType::FixedLocation)
//	{
//		SetActorLocation(CurrentMoveEffect.Location + CurrentMoveEffect.LocationOffset);
//		GoalTrasnform.SetTranslation(CurrentMoveEffect.Location + CurrentMoveEffect.LocationOffset);
//	}
//	else if (CurrentMoveEffect.CameraMoveType == ECameraMoveType::CurrentToGoal)
//	{
//		GoalTrasnform.SetTranslation(CurrentMoveEffect.Location + CurrentMoveEffect.LocationOffset);
//	}
//	else if (CurrentMoveEffect.CameraMoveType == ECameraMoveType::StartToGoal)
//	{
//		SetActorLocation(CurrentMoveEffect.StartLocation + CurrentMoveEffect.LocationOffset);
//		GoalTrasnform.SetTranslation(CurrentMoveEffect.Location + CurrentMoveEffect.LocationOffset);
//	}
//	else if (CurrentMoveEffect.CameraMoveType == ECameraMoveType::Target)
//	{
//		if (CurrentMoveEffect.LocationTargetActorType == ECameraGoalActorType::Owner) LocationActor = CurrentTurnCharacter;
//		else if (CurrentMoveEffect.LocationTargetActorType == ECameraGoalActorType::Target)
//		{
//			UAbilityComponent* asc = Cast<UAbilityComponent>(CurrentTurnCharacter->GetAbilitySystemComponent());
//			LocationActor = asc->GetTarget();
//		}
//		FAttachmentTransformRules rules(EAttachmentRule::KeepWorld, true);
//		AttachToActor(LocationActor.Get(), rules);
//		SetActorRelativeLocation(CurrentMoveEffect.LocationOffset);
//	}
//
//	if (CurrentMoveEffect.CameraLookAtType == ECameraLookAtType::SplineLocation)
//	{
//		//TODO
//	}
//	else if (CurrentMoveEffect.CameraLookAtType == ECameraLookAtType::FixedLocation)
//	{
//		SetActorRotation(
//			FQuat4d(
//				UKismetMathLibrary::FindLookAtRotation
//				(GetActorLocation(), CurrentMoveEffect.LookAtLocation + CurrentMoveEffect.LookAtOffset)
//			));
//		GoalTrasnform.SetRotation(
//			FQuat4d(
//				UKismetMathLibrary::FindLookAtRotation
//				(GetActorLocation(), CurrentMoveEffect.LookAtLocation + CurrentMoveEffect.LookAtOffset)
//			));
//	}
//	else if (CurrentMoveEffect.CameraLookAtType == ECameraLookAtType::CurrentToGoal)
//	{
//		GoalTrasnform.SetRotation(
//			FQuat4d(
//				UKismetMathLibrary::FindLookAtRotation
//				(CurrentMoveEffect.Location, CurrentMoveEffect.LookAtLocation + CurrentMoveEffect.LookAtOffset)
//			));
//	}
//	else if (CurrentMoveEffect.CameraLookAtType == ECameraLookAtType::StartToGoal)
//	{
//		SetActorRotation(
//			FQuat4d(
//				UKismetMathLibrary::FindLookAtRotation
//				(GetActorLocation(), InEffectContext->StartLookAtLocation + InEffectContext->LookAtOffset)
//			));
//		GoalTrasnform.SetRotation(
//			FQuat4d(
//				UKismetMathLibrary::FindLookAtRotation
//				(CurrentMoveEffect.Location, CurrentMoveEffect.LookAtLocation + CurrentMoveEffect.LookAtOffset)
//			));
//	}
//	else if (CurrentMoveEffect.CameraLookAtType == ECameraLookAtType::Target)
//	{
//		if (CurrentMoveEffect.LookAtTargetActorType == ECameraGoalActorType::Owner) LookAtTargetActor = CurrentTurnCharacter;
//		else if (CurrentMoveEffect.LookAtTargetActorType == ECameraGoalActorType::Target)
//		{
//			UAbilityComponent* asc = Cast<UAbilityComponent>(CurrentTurnCharacter->GetAbilitySystemComponent());
//			LookAtTargetActor = asc->GetTarget();
//		}
//	}
//
//	bRotating = 1;
//	bReturning = 0;
//}