#include "DEMOGameState.h"
#include "Global.h"

#include "TPSSubsystem.h"
#include "TurnBasedSubsystem.h"

ADEMOGameState::ADEMOGameState()
{

}

void ADEMOGameState::BeginPlay()
{
	Super::BeginPlay();
}

void ADEMOGameState::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ADEMOGameState::ChangePhase(EGameStatePhase NewPhase, UObject* Context)
{
	CheckTrue_Print(NewPhase == EGameStatePhase::MAX, "NewPhase Error");

	ReportFunc = [&]() 
	{
		CLog::Print("ReportFunc");
		CurrentPhase = NewPhase;
		EnterPhaseState(NewPhase, Context);
	};

	ExitPhase(NewPhase);
}

void ADEMOGameState::ExitPhase(EGameStatePhase NewPhase)
{
	CheckTrue_Print(CurrentPhase == EGameStatePhase::MAX, "CurrentPhase Error");

	ActiveToken = FPhaseTransitionToken::Make(CurrentPhase, NewPhase);
	OnPhaseExit[int32(CurrentPhase)].Broadcast(ActiveToken);
}

void ADEMOGameState::EnterPhaseState(EGameStatePhase NewPhase, UObject* Context)
{
	CheckTrue_Print(CurrentPhase == EGameStatePhase::MAX, "CurrentPhase Error");

	ReportFunc = [&]()
	{
		CLog::Print("ReportFunc");
		ActiveToken = FPhaseTransitionToken();
	};

	ActiveToken = FPhaseTransitionToken::Make(ActiveToken.CurrentPhase, ActiveToken.NextPhase);
	OnPhaseEnter[int32(CurrentPhase)].Broadcast(ActiveToken, Context);
}

void ADEMOGameState::RequestPhaseChange(EGameStatePhase NewPhase, UObject* Context)
{
	CheckTrue(NewPhase == CurrentPhase);
	CheckTrue(ActiveToken.IsValid());
	ChangePhase(NewPhase, Context);
}

void ADEMOGameState::ReportPhaseProgress(const FPhaseTransitionToken& Token)
{
	CheckTrue_Print(Token != ActiveToken, "Token mismatch");
	ReportFunc();
	ReportFunc.Reset();
}

