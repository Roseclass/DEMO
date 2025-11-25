#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameplayTagContainer.h"
#include "TurnBasedPhaseManager.generated.h"

/**
 * FindNextTurn	-> SelectSkill <-> SelectTarget -> PlaySequence -> EndTurn	-> FindNextTurn
 *																	½Â¸®		-> EnterField
 *																	ÆÐ¹è		-> LoadGame
 */

struct FGameplayAbilitySpec;
struct FTurnBasedFieldLayoutRow;
class ATurnbasedPhaseCamera;
class ATurnBasedCharacter;
class UTurnBasedCharacterData;
class ASelectWidgetActor;


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

	ATurnbasedPhaseCamera* Camera;
	ASelectWidgetActor* SelectWidgetActor;
	TSubclassOf<ASelectWidgetActor>SelectWidgetActorClass;
	ASelectWidgetActor* SelectTargetCursorActor;
	TSubclassOf<ASelectWidgetActor>SelectTargetCursorActorClass;
	FGameplayTag CurrentSelectedSkillTag;
	ATurnBasedCharacter* CurrentTurnCharacter;
	ATurnBasedCharacter* TargetCharacter;

	TMap<FGameplayTag, int32> SpawnRequestCountMap;
	TMap<uint8, TArray<UTurnBasedCharacterData*>> PendingSpawnMap; 
	TMap<uint8, TSet<ATurnBasedCharacter*>> SpawnedCharacterMap;
	TArray<ATurnBasedCharacter*> PendingDeadArray;
	TSet<ATurnBasedCharacter*> HandledDeadSet;
	TArray<ATurnBasedCharacter*> LocationArray[2];
protected:
public:

	//function
private:
	void SpawnCamera();
	void SpawnSelectWidget();
	void TrySpawnCharacter();
	void SpawnCharacter(uint8 TeamID, UTurnBasedCharacterData* InData);
	void PlaceActorsOnField();

	void FindNextTurn();
	void FocusSelect();
	void PlaySequence();
	UFUNCTION()void EndTurn();

	UFUNCTION()void ConfirmSelect(FGameplayTag InSkillTag, ATurnBasedCharacter* InTarget);

	void FindDeadCharacter();
	UFUNCTION()void HandleDeadCharacter();

	bool IsPlayerVictory();
	void HandlePlayerVictory();

	bool IsPlayerDefeat();
	void HandlePlayerDefeat();

protected:
public:
	void SetLevelData(FTurnBasedFieldLayoutRow* NewLevelData);
	void AddToSpawnMap(uint8 TeamID, TArray<FGameplayTag> InGameplayTags);
	void RequestSpawnCharacter(uint8 TeamID, UTurnBasedCharacterData* InData);
};