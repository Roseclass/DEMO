#include "Characters/TurnBasedCharacter.h"
#include "Global.h"

#include "Components/SceneComponent.h"

#include "GameAbilities/AbilityComponent.h"
#include "GameAbilities/AttributeSet_Character.h"

#include "Characters/TurnBasedCameraComponent.h"

ATurnBasedCharacter::ATurnBasedCharacter()
{
	CHelpers::CreateComponent<USceneComponent>(this, &SelectTargetPoint, "SelectTargetPoint", GetRootComponent());
	CHelpers::CreateComponent<USceneComponent>(this, &SelectSkillPoint, "SelectSkillPoint", GetRootComponent());
	CHelpers::CreateActorComponent<UTurnBasedCameraComponent>(this, &TurnBasedCamera, "TurnbasedCamera");
}

void ATurnBasedCharacter::BeginPlay()
{
	Super::BeginPlay();
}

void ATurnBasedCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ATurnBasedCharacter::Init(FGuid NewSaveName, UPrimaryDataAsset* DA)
{
	Super::Init(NewSaveName, DA);

	UTurnBasedCharacterData* turnbasedData = Cast<UTurnBasedCharacterData>(DA);
	CheckTrue_Print(!turnbasedData, "turnbasedData cast Fail!!");
	RuntimeData.bInitComplete = 1;
	RuntimeData = turnbasedData->RuntimeData;
	GetMesh()->SetSkeletalMesh(turnbasedData->SkeletalMesh.Get());
	GetMesh()->SetAnimationMode(EAnimationMode::AnimationBlueprint);
	GetMesh()->SetAnimInstanceClass(turnbasedData->AnimBlueprint.Get());

	//asc
	TArray<FAbilitySpecInfo> abilities;
	for (auto i : turnbasedData->GrantedAbilities)abilities.Add(i.Value);
	for (auto& i : abilities)i.SourceObject = turnbasedData;
	Ability->InitGA(abilities);
	Ability->InitAttributes(&turnbasedData->AttributeInitialInfo);
}

FGameplayTag ATurnBasedCharacter::GetDataTag() const
{
	return RuntimeData.DataTag;
}

FTransform ATurnBasedCharacter::GetSelectTargetTransform() const
{
	return FTransform();
}

FTransform ATurnBasedCharacter::GetSelectSkillTransform() const
{
	return FTransform();
}

float ATurnBasedCharacter::GetSpeed() const
{
	return Ability->GetSpeed();
}
float ATurnBasedCharacter::GetTurnGauge() const
{
	return Ability->GetTurnGauge();
}