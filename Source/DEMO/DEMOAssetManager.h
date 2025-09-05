#pragma once

#include "CoreMinimal.h"
#include "Engine/AssetManager.h"
#include "GameplayTagContainer.h"
#include "DEMOAssetManager.generated.h"

/**
 * 
 */

class UPrimaryDataAsset;

UCLASS()
class DEMO_API UDEMOAssetManager : public UAssetManager
{
	GENERATED_BODY()

public:
	static UDEMOAssetManager& Get();
	static UDEMOAssetManager* GetIfValid();
	/** Starts initial load, gets called from InitializeObjectReferences */
	virtual void StartInitialLoading() override;

private:
	TMap<FGameplayTag, UPrimaryDataAsset*>TPSDatas;
	TMap<FGameplayTag, UPrimaryDataAsset*>TurnBasedDatas;
public:
	void RequestAsyncLoad(const FSoftObjectPath& TargetToStream, TFunction<void()>&& Callback);
	void RequestAsyncLoad(TArray<FSoftObjectPath> TargetToStream, TFunction<void()>&& Callback);
};
