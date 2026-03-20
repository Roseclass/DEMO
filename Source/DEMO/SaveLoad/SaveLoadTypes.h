#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "GameplayTagContainer.h"
#include "Global.h"
#include "GameAbilities/AbilityUIEnums.h"
#include "SaveLoadTypes.generated.h"

/**
 *
 */

//#include "SaveLoad/SaveLoadTypes.h"

struct FSaveWriteKey 
{ 
friend class USaveLoadSubsystem; 
private: 
	FSaveWriteKey() = default;
};

UENUM(BlueprintType)
enum class ESaveLoadResult : uint8
{
	Success,
	Failure_Meta,
	Failure_Game,
	IndexError,
	NameError,
	MAX
};

USTRUCT(BlueprintType)
struct FSaveMetaData
{
	GENERATED_BODY()

public:
	// the name of the slot
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Default")
		FString SlotName;

	// the date that the slot was created (or the game was saved)
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Default")
		FDateTime Date;

	//
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Default")
		bool bIsEmpty = 1;
};

UCLASS()
class DEMO_API USaveGameMetaData : public USaveGame
{
	GENERATED_BODY()

public:
	UPROPERTY(VisibleAnywhere, Category = "Default")
		TArray<FSaveMetaData> SavedGameMetaDatas;

	UPROPERTY(VisibleAnywhere, Category = "Default")
		FString ActiveSlot;
};

USTRUCT(BlueprintType)
struct FSaveUIData
{
	GENERATED_BODY()

public:
	// UISetting
	UPROPERTY(VisibleAnywhere, Category = "TurnBased")
		FGameplayTag EquippedSkillTags[int32(ESkillSlotLocation::MAX)];
};

USTRUCT(BlueprintType)
struct FSaveData
{
	GENERATED_BODY()

public:
	// character setting DA
	UPROPERTY(VisibleAnywhere, Category = "Default")
		FGameplayTag DATag;

	// 
	UPROPERTY(VisibleAnywhere, Category = "Default")
		FTransform Transform;

	// 
	UPROPERTY(VisibleAnywhere, Category = "Default")
		uint8 TeamID;
};

UCLASS()
class DEMO_API USaveGameData : public USaveGame
{
	GENERATED_BODY()
public:
	static USaveGameData* CreateDefaultSaveData()
	{
		USaveGameData* saveGameData = Cast<USaveGameData>(UGameplayStatics::CreateSaveGameObject(USaveGameData::StaticClass()));
		FGuid key = FGuid::NewGuid();
		saveGameData->SavedPlayerDatas;
		saveGameData->SavedPlayerDatas.Add(key, FSaveData());
		saveGameData->SavedPlayerDatas[key].DATag = FGameplayTag::RequestGameplayTag("Data.Terra");
		saveGameData->SavedPlayerDatas[key].TeamID = TEAMID_PLAYER;
		saveGameData->SavedPlayerDatas[key].Transform.SetTranslation(FVector(900, 1120, 96));
		saveGameData->SavedPlayerDatas[key].Transform.SetRotation(FQuat4d(FRotator(0, 0, 0)));

		FSaveUIData& gideon = saveGameData->SavedPlayerUIDatas.FindOrAdd(FGameplayTag::RequestGameplayTag("Skill.Gideon"));
		gideon.EquippedSkillTags[int32(ESkillSlotLocation::Attack)] = FGameplayTag::RequestGameplayTag("Skill.Gideon.Attack");
		gideon.EquippedSkillTags[int32(ESkillSlotLocation::Skill_0)] = FGameplayTag::RequestGameplayTag("Skill.Gideon.Buff");
		gideon.EquippedSkillTags[int32(ESkillSlotLocation::Skill_1)] = FGameplayTag::RequestGameplayTag("Skill.Gideon.Debuff");
		gideon.EquippedSkillTags[int32(ESkillSlotLocation::Passive)] = FGameplayTag::RequestGameplayTag("Skill.Gideon.GrantExtraTurn");

		FSaveUIData& revenant = saveGameData->SavedPlayerUIDatas.FindOrAdd(FGameplayTag::RequestGameplayTag("Skill.Revenant"));
		revenant.EquippedSkillTags[int32(ESkillSlotLocation::Attack)] = FGameplayTag::RequestGameplayTag("Skill.Revenant.Attack");
		revenant.EquippedSkillTags[int32(ESkillSlotLocation::Skill_0)] = FGameplayTag::RequestGameplayTag("Skill.Revenant.Cast");
		revenant.EquippedSkillTags[int32(ESkillSlotLocation::Skill_1)] = FGameplayTag::RequestGameplayTag("Skill.Revenant.CastMassive");
		revenant.EquippedSkillTags[int32(ESkillSlotLocation::Passive)] = FGameplayTag::RequestGameplayTag("Skill.Revenant.CastUlt");

		FSaveUIData& terra = saveGameData->SavedPlayerUIDatas.FindOrAdd(FGameplayTag::RequestGameplayTag("Skill.Terra"));
		terra.EquippedSkillTags[int32(ESkillSlotLocation::Attack)] = FGameplayTag::RequestGameplayTag("Skill.Terra.Attack");
		terra.EquippedSkillTags[int32(ESkillSlotLocation::Skill_0)] = FGameplayTag::RequestGameplayTag("Skill.Terra.ProtectBuff");
		terra.EquippedSkillTags[int32(ESkillSlotLocation::Skill_1)] = FGameplayTag::RequestGameplayTag("Skill.Terra.CounterAttack");
		terra.EquippedSkillTags[int32(ESkillSlotLocation::Passive)] = FGameplayTag::RequestGameplayTag("Skill.Terra.Stun");

		key = FGuid::NewGuid();
		saveGameData->SavedEnemyDatas;
		saveGameData->SavedEnemyDatas.Add(key, FSaveData());
		saveGameData->SavedEnemyDatas[key].DATag = FGameplayTag::RequestGameplayTag("Data.Morigesh");
		saveGameData->SavedEnemyDatas[key].TeamID = TEAMID_ENEMY;
		saveGameData->SavedEnemyDatas[key].Transform.SetTranslation(FVector(1400, 1120, 96));
		saveGameData->SavedEnemyDatas[key].Transform.SetRotation(FQuat4d(FRotator(0, 180, 0)));

		return saveGameData;
	}
public:
	UPROPERTY(VisibleAnywhere, Category = "Default")
		TMap<FGuid, FSaveData> SavedPlayerDatas;

	UPROPERTY(VisibleAnywhere, Category = "Default")
		TMap<FGameplayTag, FSaveUIData> SavedPlayerUIDatas; // {CharacterDataTag,UIData}

	UPROPERTY(VisibleAnywhere, Category = "Default")
		TMap<FGuid, FSaveData> SavedEnemyDatas;
};