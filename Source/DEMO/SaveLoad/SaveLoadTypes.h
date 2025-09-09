#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "GameplayTagContainer.h"
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
	UPROPERTY(VisibleAnywhere, Category = "Default")
		TArray<FSaveData> SavedPlayerDatas;

	UPROPERTY(VisibleAnywhere, Category = "Default")
		TArray<FSaveData> SavedEnemyDatas;
};