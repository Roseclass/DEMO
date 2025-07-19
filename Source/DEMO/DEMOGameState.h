#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "DEMOGameState.generated.h"

/**
 * 
 */

class UAbilityComponent;

UCLASS()
class DEMO_API ADEMOGameState : public AGameStateBase
{
	GENERATED_BODY()

public:
protected:
public:

	//property
private:
protected:
	UPROPERTY(VisibleDefaultsOnly)
		UAbilityComponent* Skill;
public:

	//function
private:
protected:
public:
};

/*
* 어빌리티를 이용해 페이즈를 전환할것
* 
*/