#include "DEMOGameState.h"
#include "Global.h"

#include "MainMenuSubsystem.h"
#include "TPSSubsystem.h"
#include "TurnBasedSubsystem.h"
#include "SaveLoadSubsystem.h"

ADEMOGameState::ADEMOGameState()
{

}

void ADEMOGameState::BeginPlay()
{
	Super::BeginPlay();
	Init();
}

void ADEMOGameState::Init()
{
	UMainMenuSubsystem* MM = GetGameInstance()->GetSubsystem<UMainMenuSubsystem>();
	if (MM)MM->InitPhaseSystem();
	UTPSSubsystem* TPS = GetGameInstance()->GetSubsystem<UTPSSubsystem>();
	if (TPS)TPS->InitPhaseSystem();
	UTurnBasedSubsystem* TBS = GetGameInstance()->GetSubsystem<UTurnBasedSubsystem>();
	if (TBS)TBS->InitPhaseSystem();
	USaveLoadSubsystem* SLS = GetGameInstance()->GetSubsystem<USaveLoadSubsystem>();
	if (SLS)SLS->Init();
}

void ADEMOGameState::ChangePhase(EGameStatePhase NewPhase, UObject* Context)
{
	CheckTrue_Print(NewPhase == EGameStatePhase::MAX, "NewPhase Error");

	ReportFunc = [&]() 
	{
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

