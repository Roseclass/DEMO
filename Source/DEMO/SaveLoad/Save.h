#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "Save.generated.h"

class USaveGameData;

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class USave : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class DEMO_API ISave
{
	GENERATED_BODY()

public:
	// Get a unique name to use when saving
	virtual	FGuid GetUniqueSaveName() const = 0;

	// Called right before the owning actor is saved
	virtual	void OnBeforeSave(USaveGameData* SaveData) = 0;

	// Called after the load
	virtual	void OnAfterLoad(USaveGameData* ReadData) = 0;
};