#include "DEMOGameMode.h"
#include "DEMOCharacter.h"
#include "UObject/ConstructorHelpers.h"

ADEMOGameMode::ADEMOGameMode()
{
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPerson/Blueprints/BP_ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}
