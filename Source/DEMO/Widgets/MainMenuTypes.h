#pragma once

#include "MainMenuTypes.generated.h"

//#include "Widgets/MainMenuTypes.h"

UENUM(BlueprintType)
enum class EMainMenuPhase : uint8
{
	Continue,
	NewGame,
	LoadGame,
	Settings,
	ExitGame,
	MAX
};
