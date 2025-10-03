#include "Characters/TurnBasedCharacter.h"
#include "Global.h"

#include "SaveLoadSubsystem.h"

#include "GameAbilities/AbilityComponent.h"
#include "GameAbilities/AttributeSet_Character.h"

#include "Characters/TurnBasedCameraComponent.h"

ATurnBasedCharacter::ATurnBasedCharacter()
{
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

	//init asset
	GetMesh()->SetSkeletalMesh(turnbasedData->SkeletalMesh.Get());
	GetMesh()->SetAnimationMode(EAnimationMode::AnimationBlueprint);
	GetMesh()->SetAnimInstanceClass(turnbasedData->AnimBlueprint.Get());

	USaveLoadSubsystem* SLS = GetGameInstance()->GetSubsystem<USaveLoadSubsystem>();
	const FSaveUIData* uiData = &SLS->ReadGameData()->SavedPlayerUIDatas.FindOrAdd(turnbasedData->SkillRootTag);

	//runtimedata
	RuntimeData = turnbasedData->RuntimeData;
	RuntimeData.bInitComplete = 1;
	for (int32 i = 0; i < int32(ESkillSlotLocation::MAX); i++)
		RuntimeData.EquippedSkillTags[i] = uiData->EquippedSkillTags[i];

	//asc
	TArray<FAbilitySpecInfo> abilities;
	for (auto i : uiData->EquippedSkillTags)
	{
		if (!turnbasedData->GrantedAbilities.Contains(i))continue;
		abilities.Add(turnbasedData->GrantedAbilities[i]);
	}
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