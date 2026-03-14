#include "TPSSubsystem.h"
#include "Global.h"

#include "DEMOAssetManager.h"
#include "DEMOGameInstance.h"
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
	
	//UNDONE:: 플레이어 스왑 가능한 메시들 어떻게 추가할건지?
	{
		FGameplayTag tag = data->SavedPlayerDatas.begin()->Value.DATag;
		FGuid saveName = data->SavedPlayerDatas.begin()->Key;
		TArray<FSoftObjectPath> ptrArr;
		ptrArr.Add(Registry->CharacterDAMap[tag]->SkeletalMesh.ToSoftObjectPath());
		ptrArr.Add(Registry->CharacterDAMap[tag]->AnimBlueprint.ToSoftObjectPath());

		TFunction<void()> bind = [this, saveName, tag]()
		{
			if (!Manager)
			{
				CLog::Print("???");
				return;
			}
			Manager->InitPlayerCharacter(saveName, Registry->CharacterDAMap[tag]);
		};
		UDEMOAssetManager::GetIfValid()->RequestAsyncLoad(ptrArr, MoveTemp(bind));
	}

	for (auto i : data->SavedEnemyDatas)
	{
		FGameplayTag tag = i.Value.DATag;
		FGuid saveName = i.Key;
		if (!Registry->CharacterDAMap.Contains(tag))
		{
			CLog::Print(__FUNCTION__ + tag.ToString() + "asset not contains in subsystem");
			continue;
		}

		TArray<FSoftObjectPath> ptrArr;
		ptrArr.Add(Registry->CharacterDAMap[tag]->SkeletalMesh.ToSoftObjectPath());
		ptrArr.Add(Registry->CharacterDAMap[tag]->AnimBlueprint.ToSoftObjectPath());

		TFunction<void()> bind = [this, saveName, tag]()
		{
			if (!Manager)
			{
				CLog::Print("???");
				return;
			}
			Manager->RequestLoadCharacter(saveName, Registry->CharacterDAMap[tag]);
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
	if (ActiveToken.CurrentPhase == EGameInstancePhase::MainMenu)
	{
		UGameplayStatics::OpenLevel(GetWorld(), FName(TEXT("DEMO")));
		LoadMapDelegateHandle = FCoreUObjectDelegates::PostLoadMapWithWorld.AddLambda([this](UWorld* World)
			{
				InitializeTPSField();
			}
		);
	}
	//턴제->TPS
	else if (ActiveToken.CurrentPhase == EGameInstancePhase::TurnBased)
	{
		// enemy ai 다시 작동, tps 인풋 활성화

		//카메라 정상화
		APlayerController* pc = UGameplayStatics::GetPlayerController(GetWorld(), 0);
		pc->SetInputMode(FInputModeGameOnly());
		pc->SetViewTargetWithBlend(pc->GetPawn());
	}

	UDEMOGameInstance* gi = Cast<UDEMOGameInstance>(GetGameInstance());
	gi->ReportPhaseProgress(ActiveToken);
}

void UTPSSubsystem::ExitTPS(FPhaseTransitionToken InToken)
{
	ActiveToken = InToken;

	//TPS->메인메뉴
	if (ActiveToken.NextPhase == EGameInstancePhase::MainMenu)
	{
		// 딱히할게없는데?
	}
	else if (ActiveToken.NextPhase == EGameInstancePhase::TurnBased)
	{
		// 저장은 어디서?
		// enemy ai 멈추기, tps 인풋 멈추기
	}

	UDEMOGameInstance* gi = Cast<UDEMOGameInstance>(GetGameInstance());
	gi->ReportPhaseProgress(ActiveToken);
}

void UTPSSubsystem::InitPhaseSystem()
{
	CheckTrue(bInitPhaseSystem);
	bInitPhaseSystem = 1;
	UDEMOGameInstance* gi = Cast<UDEMOGameInstance>(GetGameInstance());
	gi->OnPhaseEnter[int32(EGameInstancePhase::TPS)].AddUFunction(this, "EnterTPS");
	gi->OnPhaseExit[int32(EGameInstancePhase::TPS)].AddUFunction(this, "ExitTPS");
}
