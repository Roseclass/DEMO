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

	if (!SpawnQueue.IsEmpty())TrySpawnCharacter();
}

void ATPSPhaseManager::SpawnCharacter(FGuid InSaveName, UTPSCharacterData* InData)
{
	CheckNull(InData);
	ATPSCharacter* ch = GetWorld()->SpawnActorDeferred<ATPSCharacter>(ATPSCharacter::StaticClass(), FTransform());
	UGameplayStatics::FinishSpawningActor(ch, FTransform());
	ch->Init(InSaveName, InData);
	CharacterSet.Add(ch);

	USaveLoadSubsystem* SLS = GetGameInstance()->GetSubsystem<USaveLoadSubsystem>();
	USaveGameData* data = SLS->ReadGameData();
	if (data->SavedPlayerDatas.Contains(InSaveName) ||
		data->SavedEnemyDatas.Contains(InSaveName))
	{
		ch->OnAfterLoad(data);
	}
}

void ATPSPhaseManager::TrySpawnCharacter()
{
	USaveLoadSubsystem* SLS = GetGameInstance()->GetSubsystem<USaveLoadSubsystem>();
	USaveGameData* saveData = SLS->ReadGameData();

	while (!SpawnQueue.IsEmpty())
	{
		FGuid name = SpawnQueue[0].Key;
		UTPSCharacterData* data = SpawnQueue[0].Value;
		SpawnQueue.RemoveAt(0);
		SpawnCharacter(name, data);
	}
}

void ATPSPhaseManager::InitPlayerCharacter(FGuid InSaveName, UTPSCharacterData* InData)
{
	ATPSCharacter* ch = Cast<ATPSCharacter>(UGameplayStatics::GetPlayerControllerFromID(GetWorld(), 0)->GetPawn());
	ch->Init(InSaveName, InData);

	USaveLoadSubsystem* SLS = GetGameInstance()->GetSubsystem<USaveLoadSubsystem>();
	USaveGameData* data = SLS->ReadGameData();
	if (data->SavedPlayerDatas.Contains(InSaveName) ||
		data->SavedEnemyDatas.Contains(InSaveName))
	{
		ch->OnAfterLoad(data);
	}
}

void ATPSPhaseManager::RequestLoadCharacter(FGuid InSaveName, UTPSCharacterData* InData)
{
	SpawnQueue.Add({ InSaveName,InData });
}

void ATPSPhaseManager::RequestSpawnCharacter(UTPSCharacterData* InData)
{
	SpawnQueue.Add({ FGuid(),InData });
}