#include "DEMOGameInstance.h"
#include "Global.h"

#include "MainMenuSubsystem.h"
#include "TPSSubsystem.h"
#include "TurnBasedSubsystem.h"
#include "SaveLoadSubsystem.h"

void UDEMOGameInstance::Init()
{
	Super::Init();	

	UMainMenuSubsystem* MM = GetSubsystem<UMainMenuSubsystem>();
	if (MM)MM->InitPhaseSystem();
	UTPSSubsystem* TPS = GetSubsystem<UTPSSubsystem>();
	if (TPS)TPS->InitPhaseSystem();
	UTurnBasedSubsystem* TBS = GetSubsystem<UTurnBasedSubsystem>();
	if (TBS)TBS->InitPhaseSystem();
	USaveLoadSubsystem* SLS = GetSubsystem<USaveLoadSubsystem>();
	if (SLS)SLS->Init();

}

void UDEMOGameInstance::ChangePhase(EGameInstancePhase NewPhase, UObject* Context)
{
	CheckTrue_Print(NewPhase == EGameInstancePhase::MAX, "NewPhase Error");

	ReportFunc = [&]()
	{
		CurrentPhase = NewPhase;
		EnterPhaseState(NewPhase, Context);
	};

	ExitPhase(NewPhase);
}

void UDEMOGameInstance::ExitPhase(EGameInstancePhase NewPhase)
{
	CheckTrue_Print(CurrentPhase == EGameInstancePhase::MAX, "CurrentPhase Error");

	ActiveToken = FPhaseTransitionToken::Make(CurrentPhase, NewPhase);
	OnPhaseExit[int32(CurrentPhase)].Broadcast(ActiveToken);
}

void UDEMOGameInstance::EnterPhaseState(EGameInstancePhase NewPhase, UObject* Context)
{
	CheckTrue_Print(CurrentPhase == EGameInstancePhase::MAX, "CurrentPhase Error");

	ReportFunc = [&]()
	{
		ActiveToken = FPhaseTransitionToken();
	};

	ActiveToken = FPhaseTransitionToken::Make(ActiveToken.CurrentPhase, ActiveToken.NextPhase);
	OnPhaseEnter[int32(CurrentPhase)].Broadcast(ActiveToken, Context);
}

void UDEMOGameInstance::RequestPhaseChange(EGameInstancePhase NewPhase, UObject* Context)
{
	CheckTrue(NewPhase == CurrentPhase);
	CheckTrue(ActiveToken.IsValid());
	ChangePhase(NewPhase, Context);
}

void UDEMOGameInstance::ReportPhaseProgress(const FPhaseTransitionToken& Token)
{
	CheckTrue_Print(Token != ActiveToken, "Token mismatch");
	ReportFunc();
	ReportFunc.Reset();
}
