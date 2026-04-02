#pragma once

#include "CoreMinimal.h"
#include "Characters/BaseCharacter.h"
#include "Characters/TurnBasedCharacterData.h"
#include "GameAbilities/GameplayEffectPayloads.h"
#include "TurnBasedCharacter.generated.h"

/**
 * 
 */

class UTurnBasedCameraComponent;
class UTBActiveFXComponent;
struct FScriptedMoveContext;

DECLARE_MULTICAST_DELEGATE_OneParam(FOnAnimTagChanged, FGameplayTag);

UENUM(BlueprintType)
enum class EHighlightType : uint8
{
	NONE, Gray, Green, Red, MAX UMETA(Hidden)
};

UCLASS()
class DEMO_API ATurnBasedCharacter : public ABaseCharacter
{
	GENERATED_BODY()
public:
	ATurnBasedCharacter();
protected:
	virtual void BeginPlay() override;
public:
	virtual void Tick(float DeltaTime) override;

	//property
private:
	TArray<FScriptedMoveData> PendingScriptedMoves;
protected:
	FTurnBasedCharacterRuntimeData RuntimeData;
	//scene

	//actor
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
		UTurnBasedCameraComponent* TurnBasedCamera;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
		UTBActiveFXComponent* ActiveFX;

public:
	FOnAnimTagChanged OnAnimTagChanged;

	//function
private:
protected:
	virtual void InitAssets(UPrimaryDataAsset* DA);
	virtual void InitGA(UPrimaryDataAsset* DA);
	void ProcessScriptedMove(float DeltaTime);
public:
	virtual void Init(FGuid NewSaveName, UPrimaryDataAsset* DA) override;
	virtual FGameplayTag GetDataTag() const override;

	void EnqueueScriptedMove(const FScriptedMoveContext* InEffectContext);
	void ApplyHighlight(EHighlightType HighlightType);

	//from runtime
	FTransform GetSelectTargetTransform() const;
	FTransform GetSelectSkillTransform() const;
	FTransform GetSelectSkillRelativeTransform() const;
	TArray<FGameplayTag> GetEquippedSkillTags() const;

	//from asc
	float GetSpeed() const;
	float GetTurnGauge() const;

	void PrintAbilities();

	bool IsDead();
};
