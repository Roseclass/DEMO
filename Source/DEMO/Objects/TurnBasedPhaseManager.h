#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameplayTagContainer.h"
#include "GameAbilities/GameplayEffectContexts.h"
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
class UUW_TBRoot;
enum class ESkillTargetType : uint8;

UENUM()
enum class EActionStage : uint8
{
	FindNextTurn,
	HandleEffects,
	FindDoTDamage,
	HandleDoTDamage,
	HandleCC,
	SelectSkill,
	SelectTarget,
	PrePlaySequence,
	PlaySequence,
	EndTurn,
	FindDeadCharacter,
	HandleDeadCharacter,
	MAX UMETA(meta = (Hidden))
};

USTRUCT()
struct FReservedAction
{
	GENERATED_BODY()
public:
	ATurnBasedCharacter* Instigator;
	TArray<ATurnBasedCharacter*> TargetCharacters;
	EReservedActionType Type;
	FGameplayTag SkillTag;
};

USTRUCT()
struct FTurnOrderCheckpoint
{
	GENERATED_BODY()
public:
	TMap<ATurnBasedCharacter*, float> GaugeMap;
	ATurnBasedCharacter* CurrentTurnCharacter;
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
	TArray<FActiveGameplayEffectHandle> DoTHandles;

	TMap<EReservedActionTiming, TArray<FReservedAction>>ReservedActions;

	ATurnbasedPhaseCamera* Camera;

	UPROPERTY()UUW_TBRoot* RootWidget;
	TSubclassOf<UUW_TBRoot> RootWidgetClass;

	FGameplayTag CurrentSelectedSkillTag;
	ATurnBasedCharacter* CurrentTurnCharacter;
	TArray<ATurnBasedCharacter*> TargetCharacters;
	TArray<ATurnBasedCharacter*> AvailableTargets;

	float TargetGoalCount;
	ESkillTargetType SkillTargetType;

	TMap<FGameplayTag, int32> SpawnRequestCountMap;
	TMap<uint8, TArray<UTurnBasedCharacterData*>> PendingSpawnMap; 
	TMap<uint8, TSet<ATurnBasedCharacter*>> SpawnedCharacterMap;
	TArray<ATurnBasedCharacter*> PendingDeadArray;
	TSet<ATurnBasedCharacter*> HandledDeadSet;
	TArray<ATurnBasedCharacter*> LocationArray[2];
	TMap<ATurnBasedCharacter*, FTransform> TransformOrigin;

	TArray<FTurnOrderCheckpoint> TurnOrder;
protected:
public:

	//function
private:
	void SpawnCamera();
	void CreateRootWidget();
	void TrySpawnCharacter();
	void SpawnCharacter(uint8 TeamID, UTurnBasedCharacterData* InData);
	UFUNCTION() void OnSpawnedCharacterBeginCursorOver(AActor* TouchedActor);
	UFUNCTION() void OnSpawnedCharacterEndCursorOver(AActor* TouchedActor);
	UFUNCTION() void OnSpawnedCharacterClicked(AActor* TouchedActor, FKey ButtonPressed);
	void PlaceActorsOnField();

	void HighlightAvailableTargets();
	void ClearAvailableTargets();
	void UpdateSidebar();

	EReservedActionType ConsumeAfterCurrentAction();
	EReservedActionType ConsumeStartOfNextTurn();
	EReservedActionType ConsumeEndOfTurn();

	UFUNCTION()void HandleStageTransition();
	void FindNextTurn();
	void HandleEffects();
	void FindDoTDamage();
	void HandleDoTDamage();
	void HandleCC();
	void SelectSkill();
	void SelectTarget();
	void PlaySequence(FGameplayTag SkillTag, ATurnBasedCharacter* SkillOwner, TArray<ATurnBasedCharacter*> TargetCharacters, bool Extra = 0);
	UFUNCTION()void EndTurn();

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
	void ApplyGE(const FApplyGEContext* InEffectContext);
	void ApplyCameraMove(const FMoveCameraContext* InEffectContext);
	void ReserveAction(const FReserveActionContext* InEffectContext);
	void ChangeTarget(TArray<ATurnBasedCharacter*> PreTargets, ATurnBasedCharacter* NewTarget);
	void EnqueueScriptedMove(const FScriptedMoveContext* InEffectContext);

	const TSet<ATurnBasedCharacter*>& GetPlayerCharacters() const;
	const TSet<ATurnBasedCharacter*>& GetEnemyCharacters() const;
};