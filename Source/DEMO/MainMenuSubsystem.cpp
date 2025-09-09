#include "MainMenuSubsystem.h"
#include "Global.h"

#include "DEMOAssetManager.h"
#include "DEMOGameInstance.h"
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
	if (ActiveToken.CurrentPhase == EGameInstancePhase::TPS)
	{
		UGameplayStatics::OpenLevel(GetWorld(), FName(TEXT("LV_MainMenu")));
	}
	//턴제->메인메뉴
	else if (ActiveToken.CurrentPhase == EGameInstancePhase::TurnBased)
	{
		UGameplayStatics::OpenLevel(GetWorld(), FName(TEXT("LV_MainMenu")));
	}

	UDEMOGameInstance* gi = Cast<UDEMOGameInstance>(GetGameInstance());
	gi->ReportPhaseProgress(ActiveToken);
}

void UMainMenuSubsystem::ExitMainMenu(FPhaseTransitionToken InToken)
{
	ActiveToken = InToken;

	//메인메뉴->TPS
	if (ActiveToken.NextPhase == EGameInstancePhase::TPS)
	{

	}
	//메인메뉴->턴제 (사용금지)
	else if (ActiveToken.NextPhase == EGameInstancePhase::TurnBased)
	{
		CheckTrue_Print(1, "Next Phase is TurnBased");
	}

	UDEMOGameInstance* gi = Cast<UDEMOGameInstance>(GetGameInstance());
	gi->ReportPhaseProgress(ActiveToken);
}

void UMainMenuSubsystem::InitPhaseSystem()
{
	CheckTrue(bInitPhaseSystem);
	bInitPhaseSystem = 1;
	UDEMOGameInstance* gi = Cast<UDEMOGameInstance>(GetGameInstance());
	gi->OnPhaseEnter[int32(EGameInstancePhase::MainMenu)].AddUFunction(this, "EnterMainMenu");
	gi->OnPhaseExit[int32(EGameInstancePhase::MainMenu)].AddUFunction(this, "ExitMainMenu");
}
