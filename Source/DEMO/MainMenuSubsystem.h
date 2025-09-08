#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Datas/GameStateTypes.h"
#include "MainMenuSubsystem.generated.h"

/**
 * 
 */

UCLASS()
class DEMO_API UMainMenuSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()
public:
	UMainMenuSubsystem();
protected:
public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	//property
private:
	bool bInitPhaseSystem;
	FPhaseTransitionToken ActiveToken;
protected:
public:

	//function
private:
	UFUNCTION()void EnterMainMenu(FPhaseTransitionToken InToken, UObject* Context);
	UFUNCTION()void ExitMainMenu(FPhaseTransitionToken InToken);
protected:
public:
	void InitPhaseSystem();
};
