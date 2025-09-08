#include "TPSSubsystem.h"
#include "Global.h"

#include "DEMOAssetManager.h"
#include "DEMOGameState.h"
#include "SaveLoadSubsystem.h"

#include "Objects/TPSPhaseManager.h"

#include "Characters/TPSCharacterData.h"

UTPSSubsystem::UTPSSubsystem()
{
	UTPSSubsystemRegistry* ptr;
	CHelpers::GetAsset<UTPSSubsystemRegistry>(&ptr, "TPSSubsystemRegistry'/Game/Datas/DA_TPSSubsystemRegistry.DA_TPSSubsystemRegistry'");
	Registry = ptr;
}

void UTPSSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	for (auto& i : Registry->CharacterDAMap)
		i.Value->RuntimeData.DataTag = i.Key;

}

void UTPSSubsystem::InitializeTPSField()
{
	if (Manager)Manager->Destroy();

	Manager = GetWorld()->SpawnActorDeferred<ATPSPhaseManager>(ATPSPhaseManager::StaticClass(), FTransform());
	UGameplayStatics::FinishSpawningActor(Manager, FTransform());
	USaveLoadSubsystem* SLS = GetGameInstance()->GetSubsystem<USaveLoadSubsystem>();
	USaveGameData* data = SLS->ReadGameData();
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
			if (!Manager)
			{
				CLog::Print("???");
				return;
			}
			Manager->RequestLoadCharacter(Registry->CharacterDAMap[i.DATag]);
		};
		UDEMOAssetManager::GetIfValid()->RequestAsyncLoad(ptrArr, MoveTemp(bind));
	}

	{
		UGameplayStatics::GetPlayerController(GetWorld(), 0)->SetInputMode(FInputModeGameOnly());
	}
	FCoreUObjectDelegates::PostLoadMapWithWorld.Remove(LoadMapDelegateHandle);
}

void UTPSSubsystem::EnterTPS(FPhaseTransitionToken InToken, UObject* Context)
{
	ActiveToken = InToken;	
	//메인메뉴->TPS
	if (ActiveToken.CurrentPhase == EGameStatePhase::MainMenu)
	{
		UGameplayStatics::OpenLevel(GetWorld(), FName(TEXT("DEMO")));
		LoadMapDelegateHandle = FCoreUObjectDelegates::PostLoadMapWithWorld.AddLambda([this](UWorld* World)
			{
				InitializeTPSField();
			}
		);
	}
	//턴제->TPS
	else if (ActiveToken.CurrentPhase == EGameStatePhase::TurnBased)
	{
		// enemy ai 다시 작동, tps 인풋 활성화
	}

	ADEMOGameState* gs = Cast<ADEMOGameState>(UGameplayStatics::GetGameState(GetWorld()));
	gs->ReportPhaseProgress(ActiveToken);
}

void UTPSSubsystem::ExitTPS(FPhaseTransitionToken InToken)
{
	ActiveToken = InToken;

	//TPS->메인메뉴
	if (ActiveToken.NextPhase == EGameStatePhase::MainMenu)
	{
		// 딱히할게없는데?
	}
	else if (ActiveToken.NextPhase == EGameStatePhase::TurnBased)
	{
		// 저장은 어디서?
		// enemy ai 멈추기, tps 인풋 멈추기
	}

	ADEMOGameState* gs = Cast<ADEMOGameState>(UGameplayStatics::GetGameState(GetWorld()));
	gs->ReportPhaseProgress(ActiveToken);
}

void UTPSSubsystem::InitPhaseSystem()
{
	CheckTrue(bInitPhaseSystem);
	bInitPhaseSystem = 1;
	ADEMOGameState* gs = Cast<ADEMOGameState>(UGameplayStatics::GetGameState(GetWorld()));
	gs->OnPhaseEnter[int32(EGameStatePhase::TPS)].AddUFunction(this, "EnterTPS");
	gs->OnPhaseExit[int32(EGameStatePhase::TPS)].AddUFunction(this, "ExitTPS");
}
