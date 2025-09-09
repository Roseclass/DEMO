#include "Objects/TPSPhaseManager.h"
#include "Global.h"

#include "SaveLoadSubsystem.h"

#include "Characters/TPSCharacter.h"

ATPSPhaseManager::ATPSPhaseManager()
{
	PrimaryActorTick.bCanEverTick = true;
}

void ATPSPhaseManager::BeginPlay()
{
	Super::BeginPlay();
}

void ATPSPhaseManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!SpawnQueue.IsEmpty())TrySpawnCharacter(SpawnQueue);
	if (!LoadQueue.IsEmpty())TrySpawnCharacter(LoadQueue, 1);
}

void ATPSPhaseManager::SpawnCharacter(UTPSCharacterData* InData, bool bLoad)
{
	CheckNull(InData);
	ATPSCharacter* ch = GetWorld()->SpawnActorDeferred<ATPSCharacter>(ATPSCharacter::StaticClass(), FTransform());
	UGameplayStatics::FinishSpawningActor(ch, FTransform());
	ch->Init(InData);
	CharacterSet.Add(ch);

	if (bLoad)
	{
		USaveLoadSubsystem* SLS = GetGameInstance()->GetSubsystem<USaveLoadSubsystem>();
		USaveGameData* data = SLS->ReadGameData();
		ch->OnAfterLoad(data);
	}
}

void ATPSPhaseManager::InitPlayerCharacter(UTPSCharacterData* InData)
{
	ATPSCharacter* ch = Cast<ATPSCharacter>(UGameplayStatics::GetPlayerControllerFromID(GetWorld(), 0)->GetPawn());
	ch->Init(InData);

	USaveLoadSubsystem* SLS = GetGameInstance()->GetSubsystem<USaveLoadSubsystem>();
	USaveGameData* data = SLS->ReadGameData();
	ch->OnAfterLoad(data);
}

void ATPSPhaseManager::TrySpawnCharacter(TArray<UTPSCharacterData*>& InQueue, bool bLoad)
{
	USaveLoadSubsystem* SLS = GetGameInstance()->GetSubsystem<USaveLoadSubsystem>();
	USaveGameData* data = SLS->ReadGameData();

	while (!InQueue.IsEmpty())
	{
		UTPSCharacterData* top = InQueue[0];
		InQueue.RemoveAt(0);
		for (int32 i = 0; i < data->SavedPlayerDatas.Num();i++)
		{
			if (data->SavedPlayerDatas[i].DATag != top->RuntimeData.DataTag)continue;
			SpawnCharacter(top, bLoad);
			top = nullptr;
		}
		if (!top)continue;
		for (int32 i = 0; i < data->SavedEnemyDatas.Num();i++)
		{
			if (data->SavedEnemyDatas[i].DATag != top->RuntimeData.DataTag)continue;
			SpawnCharacter(top, bLoad);
		}
	}
}

void ATPSPhaseManager::RequestSpawnCharacter(UTPSCharacterData* InData)
{
	SpawnQueue.Add(InData);
}

void ATPSPhaseManager::RequestLoadCharacter(UTPSCharacterData* InData)
{
	LoadQueue.Add(InData);
}
