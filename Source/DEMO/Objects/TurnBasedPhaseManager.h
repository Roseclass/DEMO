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
	TSubclassOf<ASelectWidgetActor>SelectTargetClass;
	ASelectWidgetActor* SelectTarget;
	TSubclassOf<ASelectWidgetActor>SelectSkillClass;
	ASelectWidgetActor* SelectSkill;
	ATurnBasedCharacter* CurrentTurnCharacter;
	ATurnBasedCharacter* TargetCharacter;

	TMap<FGameplayTag, int32> SpawnRequestCountMap;
	TMap<uint8, TArray<UTurnBasedCharacterData*>> PendingSpawnMap; 
	TMap<uint8, TSet<ATurnBasedCharacter*>> SpawnedCharacterMap;
	TArray<ATurnBasedCharacter*> LocationArray[2];
protected:
public:

	//function
private:
	void SpawnCamera();
	void SpawnSelectTarget();
	void SpawnSelectSkill();
	void TrySpawnCharacter();
	void SpawnCharacter(uint8 TeamID, UTurnBasedCharacterData* InData);
	void PlaceActorsOnField();

	void FindNextTurn();
	void FocusSelectSkill();
	void FocusSelectTarget();
	void PlaySequence();
	void EndTurn();

	UFUNCTION()void ConfirmTarget(ATurnBasedCharacter* InTarget);
	void ConfirmSkill(FGameplayTag InSkillTag);
protected:
public:
	void SetLevelData(FTurnBasedFieldLayoutRow* NewLevelData);
	void AddToSpawnMap(uint8 TeamID, TArray<FGameplayTag> InGameplayTags);
	void RequestSpawnCharacter(uint8 TeamID, UTurnBasedCharacterData* InData);
};