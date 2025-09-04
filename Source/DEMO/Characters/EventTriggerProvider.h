#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "EventTriggerProvider.generated.h"

UINTERFACE(MinimalAPI)
class UEventTriggerProvider : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */

class AEventTrigger;

class DEMO_API IEventTriggerProvider
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	virtual AEventTrigger* GetEventTrigger() = 0;
	virtual void EnableTriggerCollision(bool bEnable) = 0;
};
