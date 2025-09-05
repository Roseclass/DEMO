#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "DEMOGameInstance.generated.h"

/**
 * 
 */

UCLASS()
class DEMO_API UDEMOGameInstance : public UGameInstance
{
	GENERATED_BODY()
public:
	/* virtual function to allow custom gameinstance an opportunity to set up what it needs */
	virtual void Init() override;
};
