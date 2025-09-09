#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameplayTagContainer.h"
#include "TurnBasedPhaseManager.generated.h"

/**
 * FindNextTurn -> SelectTarget <-> SelectSkill -> PlaySequence -> EndTurn	-> FindNextTurn
 *																	승리		-> EnterField
 *																	패배		-> LoadGame
 */

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
	TMap<uint8, TArray<FGameplayTag>/*중복가능*/> SpawnMap;
	TMap<uint8, TArray<UTurnBasedCharacterData*>> PendingSpawnMap; 
	TMap<uint8, TSet<ATurnBasedCharacter*>> SpawnedCharacterMap;
protected:
public:

	//function
private:
	void SpawnCharacter(uint8 TeamID, UTurnBasedCharacterData* InData);
	void TrySpawnCharacter();
protected:
public:
	void AddToSpawnMap(uint8 TeamID, TArray<FGameplayTag> InGameplayTags);
	void RequestSpawnCharacter(uint8 TeamID, UTurnBasedCharacterData* InData);
};