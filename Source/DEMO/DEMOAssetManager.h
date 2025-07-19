#pragma once

#include "CoreMinimal.h"
#include "Engine/AssetManager.h"
#include "DEMOAssetManager.generated.h"

/**
 * 
 */

UCLASS()
class DEMO_API UDEMOAssetManager : public UAssetManager
{
	GENERATED_BODY()

public:

	static UDEMOAssetManager& Get();

	/** Starts initial load, gets called from InitializeObjectReferences */
	virtual void StartInitialLoading() override;
};
