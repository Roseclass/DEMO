#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameplayTagContainer.h"
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

	TMap<EReservedActionTiming, TArray<FPayloadContext>>ReservedActions;

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
	void ApplyCameraMove(const FPayloadContext* InEffectContext);
	void ReserveAction(const FPayloadContext* InEffectContext);
	void ApplyGE(const FPayloadContext* InEffectContext);
	void ChangeTarget(ATurnBasedCharacter* InTarget);

	const TSet<ATurnBasedCharacter*>& GetPlayerCharacters() const;
	const TSet<ATurnBasedCharacter*>& GetEnemyCharacters() const;
};