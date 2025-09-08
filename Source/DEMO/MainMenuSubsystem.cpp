#include "MainMenuSubsystem.h"
#include "Global.h"

#include "DEMOAssetManager.h"
#include "DEMOGameState.h"
#include "SaveLoadSubsystem.h"

UMainMenuSubsystem::UMainMenuSubsystem()
{

}

void UMainMenuSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
}

void UMainMenuSubsystem::EnterMainMenu(FPhaseTransitionToken InToken, UObject* Context)
{
	ActiveToken = InToken;

	//TPS->메인메뉴
	if (ActiveToken.CurrentPhase == EGameStatePhase::TPS)
	{
		UGameplayStatics::OpenLevel(GetWorld(), FName(TEXT("LV_MainMenu")));
	}
	//턴제->메인메뉴
	else if (ActiveToken.CurrentPhase == EGameStatePhase::TurnBased)
	{
		UGameplayStatics::OpenLevel(GetWorld(), FName(TEXT("LV_MainMenu")));
	}

	ADEMOGameState* gs = Cast<ADEMOGameState>(UGameplayStatics::GetGameState(GetWorld()));
	gs->ReportPhaseProgress(ActiveToken);
}

void UMainMenuSubsystem::ExitMainMenu(FPhaseTransitionToken InToken)
{
	ActiveToken = InToken;

	//메인메뉴->TPS
	if (ActiveToken.NextPhase == EGameStatePhase::TPS)
	{

	}
	//메인메뉴->턴제 (사용금지)
	else if (ActiveToken.NextPhase == EGameStatePhase::TurnBased)
	{
		CheckTrue_Print(1, "Next Phase is TurnBased");
	}

	ADEMOGameState* gs = Cast<ADEMOGameState>(UGameplayStatics::GetGameState(GetWorld()));
	gs->ReportPhaseProgress(ActiveToken);
}

void UMainMenuSubsystem::InitPhaseSystem()
{
	CheckTrue(bInitPhaseSystem);
	bInitPhaseSystem = 1;
	ADEMOGameState* gs = Cast<ADEMOGameState>(UGameplayStatics::GetGameState(GetWorld()));
	gs->OnPhaseEnter[int32(EGameStatePhase::MainMenu)].AddUFunction(this, "EnterMainMenu");
	gs->OnPhaseExit[int32(EGameStatePhase::MainMenu)].AddUFunction(this, "ExitMainMenu");
}
