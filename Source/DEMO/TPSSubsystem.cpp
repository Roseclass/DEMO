#include "TPSSubsystem.h"
#include "Global.h"

#include "DEMOAssetManager.h"
#include "DEMOGameState.h"
#include "DEMOPlayerState.h"

#include "Objects/TPSPhaseManager.h"

#include "Characters/TPSCharacterData.h"

UTPSSubsystem::UTPSSubsystem()
{

}

void UTPSSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	for (auto& i : Registry->CharacterDAMap)
		i.Value->DataTag = i.Key;

}

void UTPSSubsystem::InitializeTPSField()
{
	if (Manager)Manager->Destroy();

	Manager = GetWorld()->SpawnActor<ATPSPhaseManager>(ATPSPhaseManager::StaticClass(), FActorSpawnParameters());

	ADEMOPlayerState* ps = Cast<ADEMOPlayerState>(UGameplayStatics::GetPlayerState(GetWorld(), 0));
	USaveGameData* data = ps->ReadGameData();
	for (auto i : data->SavedPlayerDatas)
	{
		if (!Registry->CharacterDAMap.Contains(i.DATag))
		{
			CLog::Print(__FUNCTION__ + i.DATag.ToString() + "asset not contains in subsystem");
			continue;
		}

		TArray<FSoftObjectPath> ptrArr;
		ptrArr.Add(Registry->CharacterDAMap[i.DATag]->SkeletalMesh.ToSoftObjectPath());
		ptrArr.Add(Registry->CharacterDAMap[i.DATag]->AnimBlueprint.ToSoftObjectPath());

		TFunction<void()> bind = [this, i]()
		{
			Manager->RequestLoadCharacter(Registry->CharacterDAMap[i.DATag]);
		};
		UDEMOAssetManager::GetIfValid()->RequestAsyncLoad(ptrArr, MoveTemp(bind));
	}
}

void UTPSSubsystem::EnterTPS(FPhaseTransitionToken InToken, UObject* Context)
{
	ActiveToken = InToken;	

	if (ActiveToken.CurrentPhase == EGameStatePhase::MainMenu)
	{
		InitializeTPSField();		
	}
	else if (ActiveToken.CurrentPhase == EGameStatePhase::TurnBased)
	{
		// enemy ai ¥ŸΩ√ ¿€µø, tps ¿Œ«≤ »∞º∫»≠
	}

	ADEMOGameState* gs = Cast<ADEMOGameState>(UGameplayStatics::GetGameState(GetWorld()));
	gs->ReportPhaseProgress(ActiveToken);
}

void UTPSSubsystem::ExitTPS(FPhaseTransitionToken InToken)
{
	ActiveToken = InToken;

	if (ActiveToken.NextPhase == EGameStatePhase::MainMenu)
	{
		// ππ«ÿæﬂµ ..?
	}
	else if (ActiveToken.NextPhase == EGameStatePhase::TurnBased)
	{
		// enemy ai ∏ÿ√ﬂ±‚, tps ¿Œ«≤ ∏ÿ√ﬂ±‚
	}

	ADEMOGameState* gs = Cast<ADEMOGameState>(UGameplayStatics::GetGameState(GetWorld()));
	gs->ReportPhaseProgress(ActiveToken);
}

void UTPSSubsystem::InitPhaseSystem()
{
	CheckTrue(bInitPhaseSystem);
	bInitPhaseSystem = 1;
	ADEMOGameState* gs = Cast<ADEMOGameState>(UGameplayStatics::GetGameState(GetWorld()));
	gs->OnPhaseEnter[int32(EGameStatePhase::TurnBased)].AddUFunction(this, "EnterTurnBased");
	gs->OnPhaseExit[int32(EGameStatePhase::TurnBased)].AddUFunction(this, "ExitTurnBased");
}
