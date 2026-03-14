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
	UAbilityComponent* asc = Cast<UAbilityComponent>(GetAbilitySystemComponent());
	if (GetGenericTeamId() == TEAMID_PLAYER)
	{
		FGameplayTagContainer tags;
		asc->GetOwnedGameplayTags(tags);
		for (auto i : tags)
			CLog::Print(GetName() + i.ToString(), -1, 0, FColor::Silver);
		CLog::Print(asc->GetHealth(), -1, 0, FColor::Purple);
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