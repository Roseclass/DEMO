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
	CameraBoom->bDoCollisionTest = 0;
}

void ATurnbasedPhaseCamera::BeginPlay()
{
	Super::BeginPlay();	
}

void ATurnbasedPhaseCamera::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (IsRotating())
		BlendToTarget(DeltaTime);
}

void ATurnbasedPhaseCamera::BlendToTarget(float DeltaTime)
{
	FVector loc = GetActorLocation();
	FQuat4d rot;

	// set location
	if (CurrentData.ElapsedTime <= CurrentData.LBlendEnd && CurrentData.LBlendStart <= CurrentData.ElapsedTime + DeltaTime && bLocating)
	{
		if (!GetAttachParentActor())
		{
			if (CurrentData.bAttach)
			{
				AttachToActor(OriginActor, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
				SetActorRelativeLocation(CurrentData.AttachOffset);
			}
			else if (FMath::IsNearlyEqual(CurrentData.LBlendEnd, CurrentData.LBlendStart))
				loc = GetBlendOrigin();
			else loc = UKismetMathLibrary::VInterpTo(loc, GetBlendOrigin(), DeltaTime, 4.6 / (CurrentData.LBlendEnd - CurrentData.LBlendStart));

			if (!GetAttachParentActor())
				SetActorLocation(loc);
		}
	}

	// set rotaton
	if (CurrentData.ElapsedTime <= CurrentData.RBlendEnd && CurrentData.RBlendStart <= CurrentData.ElapsedTime + DeltaTime && bRotating)
	{
		if (CurrentData.LookAtType == ECameraLookAtType::OriginToDest)
			rot = FQuat4d(UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), GetBlendDest()));
		else
			rot = FQuat4d(UKismetMathLibrary::FindLookAtRotation(GetBlendDest(), GetActorLocation()));
		SetActorRotation(rot);
	}

	// set armlength
	{
		CameraBoom->TargetArmLength = GetSpringArmLength(); 
	}


	if ((!GetAttachParentActor() && UKismetMathLibrary::Vector_Distance(GetActorLocation(), GetBlendOrigin()) < 10) || CurrentData.LBlendEnd <= CurrentData.ElapsedTime)
	{
		bLocating = 0;
		if(GetAttachParentActor())
			DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
	}

	if(CurrentData.RBlendEnd <= CurrentData.ElapsedTime)
		bRotating = 0;

	CurrentData.ElapsedTime += DeltaTime;
}

void ATurnbasedPhaseCamera::HandleShotType(const FMoveCameraContext* InEffectContext)
{
	// OriginActor
	if (CurrentData.ShotType == ECameraShotType::ActorToActor || CurrentData.ShotType == ECameraShotType::ActorToLoc)
	{
		switch (CurrentData.OriginActor)
		{
		case EPayloadActorType::EventCauser:
		{
			OriginActor = InEffectContext->EventCauserActor.Get();
			break;
		}
		case EPayloadActorType::EventTargets:
		{
			if (CurrentData.OriginActorSelectType == EPayloadTargetSelectType::Random)
				OriginActor = InEffectContext->EventTargetActors[UKismetMathLibrary::RandomIntegerInRange(0, InEffectContext->EventTargetActors.Num() - 1)].Get();
			else if (CurrentData.OriginActorSelectType == EPayloadTargetSelectType::First)
				OriginActor = InEffectContext->EventTargetActors[0].Get();
			else if (CurrentData.OriginActorSelectType == EPayloadTargetSelectType::Center)
				OriginActor = InEffectContext->EventTargetActors[InEffectContext->EventTargetActors.Num() / 2].Get();
			else if (CurrentData.OriginActorSelectType == EPayloadTargetSelectType::Last)
				OriginActor = InEffectContext->EventTargetActors[InEffectContext->EventTargetActors.Num() - 1].Get();
			break;
		}
		case EPayloadActorType::RuleSource:
		{
			OriginActor = InEffectContext->RuleSourceActor.Get();
			break;
		}
		default:break;
		}
	}

	// DestActor
	if (CurrentData.ShotType == ECameraShotType::ActorToActor || CurrentData.ShotType == ECameraShotType::LocToActor)
	{
		switch (CurrentData.DestActor)
		{
		case EPayloadActorType::EventCauser:
		{
			DestActor = InEffectContext->EventCauserActor.Get();
			break;
		}
		case EPayloadActorType::EventTargets:
		{
			if (CurrentData.DestActorSelectType == EPayloadTargetSelectType::Random)
				DestActor = InEffectContext->EventTargetActors[UKismetMathLibrary::RandomIntegerInRange(0, InEffectContext->EventTargetActors.Num() - 1)].Get();
			else if (CurrentData.DestActorSelectType == EPayloadTargetSelectType::First)
				DestActor = InEffectContext->EventTargetActors[0].Get();
			else if (CurrentData.DestActorSelectType == EPayloadTargetSelectType::Center)
				DestActor = InEffectContext->EventTargetActors[InEffectContext->EventTargetActors.Num() / 2].Get();
			else if (CurrentData.DestActorSelectType == EPayloadTargetSelectType::Last)
				DestActor = InEffectContext->EventTargetActors[InEffectContext->EventTargetActors.Num() - 1].Get();
			break;
		}
		case EPayloadActorType::RuleSource:
		{
			DestActor = InEffectContext->RuleSourceActor.Get();
			break;
		}
		default:break;
		}
	}

}

FVector ATurnbasedPhaseCamera::GetBlendOrigin()
{
	if (GetAttachParentActor())return FVector(-1e9);
	if (CurrentData.ShotType == ECameraShotType::LocToActor || CurrentData.ShotType == ECameraShotType::LocToLoc)return CurrentData.OriginLoc;
	FVector result;
	result = OriginActor->GetActorLocation();
	result += OriginActor->GetActorForwardVector() * CurrentData.OriginActorOffset.X;
	result += OriginActor->GetActorRightVector() * CurrentData.OriginActorOffset.Y;
	result += OriginActor->GetActorUpVector() * CurrentData.OriginActorOffset.Z;
	return result;
}

FVector ATurnbasedPhaseCamera::GetBlendDest()
{
	if (CurrentData.ShotType == ECameraShotType::ActorToLoc || CurrentData.ShotType == ECameraShotType::LocToLoc)return CurrentData.DestLoc;
	FVector result;
	result = DestActor->GetActorLocation();
	result += DestActor->GetActorForwardVector() * CurrentData.DestActorOffset.X;
	result += DestActor->GetActorRightVector() * CurrentData.DestActorOffset.Y;
	result += DestActor->GetActorUpVector() * CurrentData.DestActorOffset.Z;
	return result;
}

float ATurnbasedPhaseCamera::GetSpringArmLength()
{
	return CurrentData.SpringArmCurve.GetRichCurve()->Eval(CurrentData.ElapsedTime);
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
	SetActorTransform(InCurrentTurnCharacter->GetSelectSkillTransform());
}

void ATurnbasedPhaseCamera::SetTargetRotation(FRotator NewRotation)
{
	SetActorRotation(NewRotation);
}

bool ATurnbasedPhaseCamera::IsRotating()const 
{
	return bLocating || bRotating;
}

void ATurnbasedPhaseCamera::ApplyCameraMove(const FMoveCameraContext* InEffectContext)
{
	CurrentData = InEffectContext->Data;
	DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);

	HandleShotType(InEffectContext);
	bLocating = 1;
	bRotating = 1;
}
