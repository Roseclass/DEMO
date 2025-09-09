#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "Datas/GameInstanceTypes.h"
#include "DEMOGameInstance.generated.h"

/**
 * 
 */

DECLARE_MULTICAST_DELEGATE_OneParam(FOnPhaseExit, FPhaseTransitionToken);
DECLARE_MULTICAST_DELEGATE_TwoParams(FOnPhaseEnter, FPhaseTransitionToken, UObject*);

UCLASS()
class DEMO_API UDEMOGameInstance : public UGameInstance
{
	GENERATED_BODY()
public:
	/* virtual function to allow custom gameinstance an opportunity to set up what it needs */
	virtual void Init() override;

	//property
private:
	EGameInstancePhase CurrentPhase;
	FPhaseTransitionToken ActiveToken;
	TUniqueFunction<void()> ReportFunc;
protected:
public:
	FOnPhaseExit OnPhaseExit[EGameInstancePhase::MAX];
	FOnPhaseEnter OnPhaseEnter[EGameInstancePhase::MAX];

	//function
private:
	void ChangePhase(EGameInstancePhase NewPhase, UObject* Context);
	void ExitPhase(EGameInstancePhase NewPhase);
	void EnterPhaseState(EGameInstancePhase NewPhase, UObject* Context);
protected:
public:
	void RequestPhaseChange(EGameInstancePhase NewPhase, UObject* Context);
	void ReportPhaseProgress(const FPhaseTransitionToken& Token);

};