#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameplayTagContainer.h"
#include "TurnBasedPhaseManager.generated.h"

/**
 * FindNextTurn -> SelectTarget <-> SelectSkill -> PlaySequence -> EndTurn	-> FindNextTurn
 *																	½Â¸®		-> EnterField
 *																	ÆÐ¹è		-> LoadGame
 */

struct FTurnBasedFieldLayoutRow;
class ATurnBasedCharacter;
class UTurnBasedCharacterData;

UCLASS()
class DEMO_API ATurnBasedPhaseManager : public AActor
{
	GENERATED_BODY()
	
public:	
	ATurnBasedPhaseManager();
protected:
	virtual void BeginPlay() override;
public:	
	virtual void Tick(float DeltaTime) override;

	//property
private:
	FTurnBasedFieldLayoutRow* LevelData;

	TMap<FGameplayTag, int32> SpawnRequestCountMap;
	TMap<uint8, TArray<UTurnBasedCharacterData*>> PendingSpawnMap; 
	TMap<uint8, TSet<ATurnBasedCharacter*>> SpawnedCharacterMap;
protected:
public:

	//function
private:
	void TrySpawnCharacter();
	void SpawnCharacter(uint8 TeamID, UTurnBasedCharacterData* InData);
	void PlaceActorsOnField();
protected:
public:
	void SetLevelData(FTurnBasedFieldLayoutRow* NewLevelData);
	void AddToSpawnMap(uint8 TeamID, TArray<FGameplayTag> InGameplayTags);
	void RequestSpawnCharacter(uint8 TeamID, UTurnBasedCharacterData* InData);
};