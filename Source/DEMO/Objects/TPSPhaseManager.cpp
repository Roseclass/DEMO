#include "Objects/TPSPhaseManager.h"
#include "Global.h"

#include "DEMOPlayerState.h"

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
		ADEMOPlayerState* ps = Cast<ADEMOPlayerState>(UGameplayStatics::GetPlayerState(GetWorld(), 0));
		USaveGameData* data = ps->ReadGameData();
		ch->OnAfterLoad(data);
	}
}

void ATPSPhaseManager::TrySpawnCharacter(TQueue<UTPSCharacterData*>& InQueue, bool bLoad)
{
	ADEMOPlayerState* ps = Cast<ADEMOPlayerState>(UGameplayStatics::GetPlayerState(GetWorld(), 0));
	USaveGameData* data = ps->ReadGameData();

	while (!InQueue.IsEmpty())
	{
		UTPSCharacterData* top;
		InQueue.Dequeue(top);
		for (auto i : data->SavedPlayerDatas)
		{
			if (i.DATag != top->DataTag)continue;
			SpawnCharacter(top);
			top = nullptr;
		}
		if (!top)continue;

		for (auto i : data->SavedEnemyDatas)
		{
			if (i.DATag != top->DataTag)continue;
			SpawnCharacter(top);
		}
	}
}

void ATPSPhaseManager::RequestSpawnCharacter(UTPSCharacterData* InData)
{
	SpawnQueue.Enqueue(InData);
}

void ATPSPhaseManager::RequestLoadCharacter(UTPSCharacterData* InData)
{
	LoadQueue.Enqueue(InData);
}
