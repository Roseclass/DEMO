#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "Datas/GameStateTypes.h"
#include "DEMOGameState.generated.h"

/**
 * 
 */

DECLARE_MULTICAST_DELEGATE_OneParam(FOnPhaseExit, FPhaseTransitionToken);
DECLARE_MULTICAST_DELEGATE_TwoParams(FOnPhaseEnter, FPhaseTransitionToken, UObject*);

UCLASS()
class DEMO_API ADEMOGameState : public AGameStateBase
{
	GENERATED_BODY()
public:
	ADEMOGameState();
protected:
	virtual void BeginPlay() override;

	//property
private:
	EGameStatePhase CurrentPhase;
	FPhaseTransitionToken ActiveToken;
	TUniqueFunction<void()> ReportFunc;
protected:
public:
	FOnPhaseExit OnPhaseExit[EGameStatePhase::MAX];
	FOnPhaseEnter OnPhaseEnter[EGameStatePhase::MAX];

	//function
private:
	void Init();
	void ChangePhase(EGameStatePhase NewPhase, UObject* Context);
	void ExitPhase(EGameStatePhase NewPhase);
	void EnterPhaseState(EGameStatePhase NewPhase, UObject* Context);
protected:
public:
	void RequestPhaseChange(EGameStatePhase NewPhase, UObject* Context);
	void ReportPhaseProgress(const FPhaseTransitionToken& Token);
};