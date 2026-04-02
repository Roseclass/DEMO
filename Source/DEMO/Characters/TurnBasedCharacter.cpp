#include "Characters/TurnBasedCharacter.h"
#include "Global.h"

#include "Components/CapsuleComponent.h"

#include "SaveLoadSubsystem.h"

#include "GameAbilities/AbilityComponent.h"
#include "GameAbilities/AttributeSet_Character.h"
#include "GameAbilities/GameplayEffectContexts.h"

#include "Characters/TBActiveFXComponent.h"
#include "Characters/TurnBasedCameraComponent.h"

ATurnBasedCharacter::ATurnBasedCharacter()
{
	CHelpers::CreateActorComponent<UTurnBasedCameraComponent>(this, &TurnBasedCamera, "TurnbasedCamera");
	CHelpers::CreateActorComponent<UTBActiveFXComponent>(this, &ActiveFX, "ActiveFX");

	GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Block);
}

void ATurnBasedCharacter::BeginPlay()
{
	Super::BeginPlay();

	
}

void ATurnBasedCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	ProcessScriptedMove(DeltaTime);
	if (GetGenericTeamId() == TEAMID_PLAYER)
	{
		FGameplayTagContainer tags;
		Ability->GetOwnedGameplayTags(tags);
		for (auto i : tags)
			CLog::Print(GetName() + i.ToString(), -1, 0, FColor::Silver);
		CLog::Print(Ability->GetHealth(), -1, 0, FColor::Purple);
	}
	if (GetGenericTeamId() == TEAMID_ENEMY)
	{
		FGameplayTagContainer tags;
		Ability->GetOwnedGameplayTags(tags);
		for (auto i : tags)
			CLog::Print(GetName() + i.ToString(), -1, 0, FColor::Black);
		CLog::Print(Ability->GetHealth(), -1, 0, FColor::Green);
	}
}

void ATurnBasedCharacter::InitAssets(UPrimaryDataAsset* DA)
{
	UTurnBasedCharacterData* turnbasedData = Cast<UTurnBasedCharacterData>(DA);
	CheckTrue_Print(!turnbasedData, "turnbasedData cast Fail!!");

	//init asset
	GetMesh()->SetSkeletalMesh(turnbasedData->SkeletalMesh.LoadSynchronous());
	GetMesh()->SetAnimationMode(EAnimationMode::AnimationBlueprint);
	GetMesh()->SetAnimInstanceClass(turnbasedData->AnimBlueprint.LoadSynchronous());
}

void ATurnBasedCharacter::InitGA(UPrimaryDataAsset* DA)
{
	UTurnBasedCharacterData* turnbasedData = Cast<UTurnBasedCharacterData>(DA);
	CheckTrue_Print(!turnbasedData, "turnbasedData cast Fail!!");

	USaveLoadSubsystem* SLS = GetGameInstance()->GetSubsystem<USaveLoadSubsystem>();
	const FSaveUIData* uiData = &SLS->ReadGameData()->SavedPlayerUIDatas.FindOrAdd(turnbasedData->SkillRootTag);

	//runtimedata
	RuntimeData = turnbasedData->RuntimeData;
	RuntimeData.bInitComplete = 1;
	for (int32 i = 0; i < int32(ESkillSlotLocation::MAX); i++)
		RuntimeData.EquippedSkillTags[i] = uiData->EquippedSkillTags[i];

	//asc
	TArray<FAbilitySpecInfo> abilities;
	for (auto tag : uiData->EquippedSkillTags)
	{
		if (!turnbasedData->AvailableAbilities.Contains(tag))continue;
		if (!turnbasedData->AvailableAbilities[tag].AbilityClass)continue;
		abilities.Add(turnbasedData->AvailableAbilities[tag]);
	}
	for (auto& tuple : turnbasedData->GrantedAbilities)
	{
		if (!tuple.Value.AbilityClass)continue;
		abilities.Add(tuple.Value);
	}
	for (auto& info : abilities)info.SourceObject = turnbasedData;
	Ability->InitGA(abilities);
	Ability->InitAttributes(&turnbasedData->AttributeInitialInfos);
}

void ATurnBasedCharacter::ProcessScriptedMove(float DeltaTime)
{
	if (PendingScriptedMoves.IsEmpty())return;

	FScriptedMoveData& data = PendingScriptedMoves[0];

	FVector currentLoc = GetActorLocation();
	FVector goalLoc = data.TargetLocation;
	FVector direction = (goalLoc - currentLoc).GetSafeNormal();

	if (data.ElapsedTime <= data.LocationEnd && data.LocationStart <= data.ElapsedTime + DeltaTime)
	{
		if (!data.LocationInit)
		{
			data.LocationInit = 1;
			if (!FMath::IsNearlyEqual(data.LocationStart, data.LocationEnd))
				data.LocationSpeed = UKismetMathLibrary::Vector_Distance(currentLoc, goalLoc) / (data.LocationEnd - data.LocationStart);
		}
		if(FMath::IsNearlyEqual(data.LocationStart, data.LocationEnd))
			SetActorLocation(goalLoc);
		else
		{
			currentLoc += direction * data.LocationSpeed * (FMath::Min(data.LocationEnd, data.ElapsedTime + DeltaTime) - FMath::Max(data.LocationStart, data.ElapsedTime));
			if (data.LocationEnd <= data.ElapsedTime + DeltaTime)
				currentLoc = goalLoc;
			SetActorLocation(currentLoc);
		}
	}

	FRotator currentRot = GetActorRotation();
	FRotator goalRot = data.TargetRotation;

	if (data.ElapsedTime <= data.RotationEnd && data.RotationStart <= data.ElapsedTime + DeltaTime)
	{
		if (!data.RotationInit)
		{
			data.RotationInit = 1;
			if (!FMath::IsNearlyEqual(data.RotationStart, data.RotationEnd))
			{
				data.RotationDeltaPerSecond = (goalRot - currentRot).GetNormalized();
				data.RotationDeltaPerSecond.Yaw /= (data.RotationEnd - data.RotationStart);
				data.RotationDeltaPerSecond.Pitch /= (data.RotationEnd - data.RotationStart);
				data.RotationDeltaPerSecond.Roll /= (data.RotationEnd - data.RotationStart);
			}
		}
		if (FMath::IsNearlyEqual(data.RotationStart, data.RotationEnd))
			SetActorRotation(goalRot);
		else
		{
			currentRot += data.RotationDeltaPerSecond * (FMath::Min(data.RotationEnd, data.ElapsedTime + DeltaTime) - FMath::Max(data.RotationStart, data.ElapsedTime));
			if (data.RotationEnd <= data.ElapsedTime + DeltaTime)
				currentRot = goalRot;
			SetActorRotation(currentRot);
		}
	}
	data.ElapsedTime += DeltaTime;
	if(data.Duration <= data.ElapsedTime)
		PendingScriptedMoves.RemoveAt(0);
}

void ATurnBasedCharacter::Init(FGuid NewSaveName, UPrimaryDataAsset* DA)
{
	Super::Init(NewSaveName, DA);

	InitAssets(DA);
	InitGA(DA);
}
FGameplayTag ATurnBasedCharacter::GetDataTag() const
{
	return RuntimeData.DataTag;
}

void ATurnBasedCharacter::EnqueueScriptedMove(const FScriptedMoveContext* InEffectContext)
{
	FScriptedMoveData data;
	data = InEffectContext->Data;
	if(data.LocationSourceType == EScriptedMoveLocationSourceType::Actor)
	{
		if (data.LocationActorType == EPayloadActorType::RuleSource)
			data.LocationActor = InEffectContext->RuleSourceActor;
		else if (data.LocationActorType == EPayloadActorType::EventCauser)
			data.LocationActor = InEffectContext->EventCauserActor;
		else if (data.LocationActorType == EPayloadActorType::EventTargets)
		{
			if (data.LocationActorSelectType == EPayloadActorSelectType::Random)
				data.LocationActor = InEffectContext->EventTargetActors[UKismetMathLibrary::RandomIntegerInRange(0, InEffectContext->EventTargetActors.Num() - 1)];
			else if (data.LocationActorSelectType == EPayloadActorSelectType::First)
				data.LocationActor = InEffectContext->EventTargetActors[0];
			else if (data.LocationActorSelectType == EPayloadActorSelectType::Center)
				data.LocationActor = InEffectContext->EventTargetActors[InEffectContext->EventTargetActors.Num() / 2];
			else if (data.LocationActorSelectType == EPayloadActorSelectType::Last)
				data.LocationActor = InEffectContext->EventTargetActors[InEffectContext->EventTargetActors.Num() - 1];
		}

		data.TargetLocation = data.LocationActor.Get()->GetActorLocation();
		data.TargetLocation += data.LocationActor.Get()->GetActorForwardVector() * data.FrontOffset;
		data.TargetLocation += data.LocationActor.Get()->GetActorRightVector() * data.RightOffset;
		data.TargetLocation += data.LocationActor.Get()->GetActorUpVector() * data.UpOffset;
	}

	if (data.RotationSourceType != EScriptedMoveRotationSourceType::Rotation)
	{
		if (data.RotationActorType == EPayloadActorType::RuleSource)
			data.RotationActor = InEffectContext->RuleSourceActor;
		else if (data.RotationActorType == EPayloadActorType::EventCauser)
			data.RotationActor = InEffectContext->EventCauserActor;
		else if (data.RotationActorType == EPayloadActorType::EventTargets)
		{
			if (data.RotationActorSelectType == EPayloadActorSelectType::Random)
				data.RotationActor = InEffectContext->EventTargetActors[UKismetMathLibrary::RandomIntegerInRange(0, InEffectContext->EventTargetActors.Num() - 1)];
			else if (data.RotationActorSelectType == EPayloadActorSelectType::First)
				data.RotationActor = InEffectContext->EventTargetActors[0];
			else if (data.RotationActorSelectType == EPayloadActorSelectType::Center)
				data.RotationActor = InEffectContext->EventTargetActors[InEffectContext->EventTargetActors.Num() / 2];
			else if (data.RotationActorSelectType == EPayloadActorSelectType::Last)
				data.RotationActor = InEffectContext->EventTargetActors[InEffectContext->EventTargetActors.Num() - 1];
		}


		if (data.RotationSourceType == EScriptedMoveRotationSourceType::CopyActor)
		{
			data.TargetRotation = data.RotationActor.Get()->GetActorRotation();
			data.TargetRotation += data.AdditionalRotation; //이거 되는거임?
		}
		else if (data.RotationSourceType == EScriptedMoveRotationSourceType::LookAtActor)
			data.TargetRotation = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), data.RotationActor.Get()->GetActorLocation());
	}

	PendingScriptedMoves.Add(data);
}

void ATurnBasedCharacter::ApplyHighlight(EHighlightType HighlightType)
{
	if (HighlightType == EHighlightType::NONE)
	{
		GetMesh()->SetRenderCustomDepth(0);
		return;
	}
	GetMesh()->SetRenderCustomDepth(1);
	if (HighlightType == EHighlightType::Gray)
		GetMesh()->SetCustomDepthStencilValue(1);
	else if (HighlightType == EHighlightType::Green)
		GetMesh()->SetCustomDepthStencilValue(2);
	else if (HighlightType == EHighlightType::Red)
		GetMesh()->SetCustomDepthStencilValue(3);
}

FTransform ATurnBasedCharacter::GetSelectTargetTransform() const
{
	return RuntimeData.SelectTargetTransform * GetActorTransform();
}

FTransform ATurnBasedCharacter::GetSelectSkillTransform() const
{
	return RuntimeData.SelectSkillTransform * GetActorTransform();
}

FTransform ATurnBasedCharacter::GetSelectSkillRelativeTransform() const
{
	return RuntimeData.SelectSkillRelativeTransform;
}

TArray<FGameplayTag> ATurnBasedCharacter::GetEquippedSkillTags() const
{
	TArray<FGameplayTag> result;
	for (auto i : RuntimeData.EquippedSkillTags)
		result.Add(i);
	return result;
}

float ATurnBasedCharacter::GetSpeed() const
{
	return Ability->GetSpeed();
}

float ATurnBasedCharacter::GetTurnGauge() const
{
	return Ability->GetTurnGauge();
}

void ATurnBasedCharacter::PrintAbilities()
{
	TArray<FGameplayAbilitySpecHandle>arr;
	GetAbilitySystemComponent()->GetAllAbilities(arr);
	for (auto& i : arr)
	{
		FGameplayAbilitySpec* spec = GetAbilitySystemComponent()->FindAbilitySpecFromHandle(i);
		if (!spec->IsActive())continue;
		CLog::Print(spec->Ability->AbilityTags.GetByIndex(0).ToString(), -1, 0, FColor::Black);
	}
}

bool ATurnBasedCharacter::IsDead()
{
	return Ability->GetHealth() <= 1e-9;
}