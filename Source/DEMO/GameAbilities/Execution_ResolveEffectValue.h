#pragma once

#include "CoreMinimal.h"
#include "GameplayEffectExecutionCalculation.h"
#include "GameAbilities/ExecutionContextTypes.h"
#include "Execution_ResolveEffectValue.generated.h"

/**
 * 
 */

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
	void HandleResolveRules(const FExecutionContext* InContext, OUT float& Additive, OUT float& Multiplier)const;
	void HandleModifyData(const FExecutionContext* InContext, const FExecutionModifyData& InData, OUT float& Additive, OUT float& Multiplier)const;
protected:
	UPROPERTY(EditDefaultsOnly)
		TObjectPtr<UDataTable> ResolveRules;

	UPROPERTY(EditDefaultsOnly)
		EExecutionResolveType ResolveType;

	UPROPERTY(EditDefaultsOnly)
		FGameplayAttribute Attribute;

	UPROPERTY(EditDefaultsOnly)
		float BaseValue;

	UPROPERTY(EditDefaultsOnly)
		TArray<FExecutionModifyData> ModifyDatas;

public:

	//function
private:
protected:
public:

};
