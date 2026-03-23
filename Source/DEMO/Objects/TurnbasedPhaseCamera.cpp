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
	FCameraPreset* preset = CameraPresetDatas.FindOrAdd(CurrentDA->ShotType);
	FVector newLoc = preset->BaseOffset + ShotOrigin->GetActorLocation();
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

}

void ATurnbasedPhaseCamera::HandleSkillType()
{
	if (CurrentDA->LookAtType == ECameraLookAtType::Target_Primary)
	{
		//LookAtLocation = CurrentDA->TargetActors[0]->GetActorLocation();
	}
	else if (CurrentDA->LookAtType == ECameraLookAtType::Target_RandomOnce)
	{
		//LookAtLocation = CurrentDA->TargetActors
		//	[UKismetMathLibrary::RandomIntegerInRange(0, CurrentDA->TargetActors.Num() - 1)]
		//->GetActorLocation();
	}
	else if (CurrentDA->LookAtType == ECameraLookAtType::Target_Center)
	{
		//for(auto i : CurrentDA->TargetActors)
		//	LookAtLocation += i->GetActorLocation();
		//LookAtLocation /= CurrentDA->TargetActors.Num();
	}
	else if (CurrentDA->LookAtType == ECameraLookAtType::ImpactPoint)
	{
		//LookAtLocation = CurrentDA->GetHitResult()->ImpactPoint;
	}
	else if (CurrentDA->LookAtType == ECameraLookAtType::Origin_Forward)
	{
		//LookAtLocation =
		//	CurrentDA->ShotOriginActor->GetActorLocation() +
		//	CurrentDA->ShotOriginActor->GetActorForwardVector();
	}
}

void ATurnbasedPhaseCamera::Init(FTransform InTransform)
{
	InitialTransform = InTransform;
}

void ATurnbasedPhaseCamera::FocusAvailableTargets(TArray<ATurnBasedCharacter*> AvailableTargets)
{
	SetActorLocation(InitialTransform.GetTranslation());

	FVector lookat = FVector();
	for (auto i : AvailableTargets)lookat += i->GetActorLocation();
	lookat /= AvailableTargets.Num();
	SetActorRotation(UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), lookat));
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

void ATurnbasedPhaseCamera::ApplyCameraMove(const FPayloadContext* InEffectContext)
{
	CurrentDA = Cast<UDA_MoveCamera>(InEffectContext->Payload);
	CheckTrue_Print(!CurrentDA, "CurrentDA cast failed!!");

	{
		switch (CurrentDA->ShotOrigin)
		{
		case EPayloadActorType::RuleSource:ShotOrigin = InEffectContext->RuleSourceActor.Get(); break;
		case EPayloadActorType::EventCauser:ShotOrigin = InEffectContext->EventCauserActor.Get(); break;
		case EPayloadActorType::EventTarget:ShotOrigin = InEffectContext->EventTargetActor.Get(); break;
		case EPayloadActorType::EventTargets:/*TODO::*/ break;
		default:break;
		}
	}
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