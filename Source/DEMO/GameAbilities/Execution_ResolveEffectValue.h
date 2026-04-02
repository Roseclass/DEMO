#pragma once

#include "CoreMinimal.h"
#include "GameplayEffectExecutionCalculation.h"
#include "GameAbilities/ExecutionContextTypes.h"
#include "GameAbilities/GameplayEffectPayloads.h"
#include "Execution_ResolveEffectValue.generated.h"

/**
 * 
 */
class ATurnBasedCharacter;
struct FTurnBasedEffectContext;

UCLASS()
class DEMO_API UExecution_ResolveEffectValue : public UGameplayEffectExecutionCalculation
{
	GENERATED_BODY()
public:
	UExecution_ResolveEffectValue();
protected:
public:
	virtual void Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams, FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const override;

	//property
private:
	void HandleResolveRules(const FTurnBasedEffectContext* InContext, OUT float& Additive, OUT float& Multiplier)const;
	void HandleModifyData(const FTurnBasedEffectContext* InContext, const FExecutionModifyData& InData, OUT float& Additive, OUT float& Multiplier)const;
	void EvaluateModifyData(ATurnBasedCharacter* Ref, const FExecutionModifyData& InData, OUT bool& bApply, OUT float& Count)const;
	void HandleCamerMoveContext(const FTurnBasedEffectContext* InContext)const;
protected:
	UPROPERTY(EditDefaultsOnly, Category = "Data")
		TObjectPtr<UDataTable> ResolveRules;

	UPROPERTY(EditDefaultsOnly, Category = "Data")
		EExecutionResolveType ResolveType;

	UPROPERTY(EditDefaultsOnly, Category = "Data")
		FGameplayAttribute Attribute;

	UPROPERTY(EditDefaultsOnly, Category = "Data")
		float BaseValue;

	UPROPERTY(EditDefaultsOnly, Category = "Data")
		TArray<FExecutionModifyData> ModifyDatas;

	UPROPERTY(EditDefaultsOnly, Category = "FX")
		FGameplayTag FXTag = FGameplayTag::RequestGameplayTag("GameplayCue.SpawnFX");

	UPROPERTY(EditDefaultsOnly, Category = "Camera")
		FMoveCameraData CamerMoveContext;
public:

	//function
private:
protected:
public:

};
