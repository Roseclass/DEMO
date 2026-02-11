#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameplayTagContainer.h"
#include "GameAbilities/GameplayEffectContexts.h"
#include "GameAbilities/GameplayEffectPayloads.h"
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

UENUM()
enum class EActionStage : uint8
{
	FindNextTurn,
	FocusSelect,
	PlaySequence,
	EndTurn,
	FindDeadCharacter,
	HandleDeadCharacter,
	MAX UMETA(meta = (Hidden))
};

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

	EActionStage CurrentStage = EActionStage::MAX;
	EActionStage NextStage = EActionStage::MAX;

	TMap<EReservedActionTiming, TArray<FPayloadContext>>ReservedActions;

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
	void ReduceCooldown();
	void HandleDoTDamage();

	EReservedActionType ConsumeAfterCurrentAction();
	EReservedActionType ConsumeStartOfNextTurn();
	EReservedActionType ConsumeEndOfTurn();

	UFUNCTION()void HandleStageTransition();
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

public: // for subsystem
	void ApplyCameraMove(const FCameraMoveEffectContext* InEffectContext);
	void ReserveAction(const FPayloadContext* InEffectContext);

	const TSet<ATurnBasedCharacter*>& GetPlayerCharacters() const;
	const TSet<ATurnBasedCharacter*>& GetEnemyCharacters() const;
};