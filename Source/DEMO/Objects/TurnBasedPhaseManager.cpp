#include "Objects/TurnBasedPhaseManager.h"
#include "Global.h"

#include "SaveLoadSubsystem.h"

#include "Characters/TurnBasedCharacter.h"
#include "Characters/TurnBasedCharacterData.h"

#include "Datas/TurnBasedDataTypes.h"

ATurnBasedPhaseManager::ATurnBasedPhaseManager()
{
	PrimaryActorTick.bCanEverTick = true;
}

void ATurnBasedPhaseManager::BeginPlay()
{
	Super::BeginPlay();
}

void ATurnBasedPhaseManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ATurnBasedPhaseManager::TrySpawnCharacter()
{
	for (auto& tuple : PendingSpawnMap)
	{
		while (!tuple.Value.IsEmpty())
		{
			UTurnBasedCharacterData* data = tuple.Value[tuple.Value.Num() - 1];
			tuple.Value.RemoveAt(tuple.Value.Num() - 1);
			SpawnCharacter(tuple.Key, data);
		}
	}

	PlaceActorsOnField();
}

void ATurnBasedPhaseManager::SpawnCharacter(uint8 TeamID, UTurnBasedCharacterData* InData)
{
	CheckNull(InData);
	ATurnBasedCharacter* ch = GetWorld()->SpawnActorDeferred<ATurnBasedCharacter>(ATurnBasedCharacter::StaticClass(), FTransform(), nullptr, nullptr, ESpawnActorCollisionHandlingMethod::AlwaysSpawn);
	UGameplayStatics::FinishSpawningActor(ch, FTransform());
	ch->Init(FGuid(), InData);
	SpawnedCharacterMap.FindOrAdd(TeamID).Add(ch);
}

void ATurnBasedPhaseManager::PlaceActorsOnField()
{
	CheckTrue_Print(!LevelData, "LevelData is nullptr");

	TArray<FVector> arr;
	int32 idx = 1;
	{
		FVector gap;
		gap = (LevelData->PlayerEnd - LevelData->PlayerStart)
			/ (SpawnedCharacterMap[TEAMID_PLAYER].Num() + 1);
		arr.Add(LevelData->PlayerStart);
		for (int32 i = 0; i < SpawnedCharacterMap[TEAMID_PLAYER].Num(); i++)
			arr.Add(arr[arr.Num() - 1] + gap);
		arr.Add(LevelData->PlayerEnd);
		for (auto i : SpawnedCharacterMap[TEAMID_PLAYER])
		{
			i->SetActorLocation(arr[idx++]);
			i->SetActorRotation(LevelData->PlayerRotation);
		}
	}
	arr.Empty(); idx = 1;
	{
		FVector gap;
		gap = (LevelData->EnemyEnd - LevelData->EnemyStart)
			/ (SpawnedCharacterMap[TEAMID_ENEMY].Num() + 1);
		arr.Add(LevelData->EnemyStart);
		for (int32 i = 0; i < SpawnedCharacterMap[TEAMID_ENEMY].Num(); i++)
			arr.Add(arr[arr.Num() - 1] + gap);
		arr.Add(LevelData->EnemyEnd);		
		for (auto i : SpawnedCharacterMap[TEAMID_ENEMY])
		{
			i->SetActorLocation(arr[idx++]);
			i->SetActorRotation(LevelData->EnemyRotation);
		}
	}
}

void ATurnBasedPhaseManager::SetLevelData(FTurnBasedFieldLayoutRow* NewLevelData)
{
	LevelData = NewLevelData;
}

void ATurnBasedPhaseManager::AddToSpawnMap(uint8 TeamID, TArray<FGameplayTag> InGameplayTags)
{
	for (auto tag : InGameplayTags)
		SpawnRequestCountMap.FindOrAdd(tag)++;
}

void ATurnBasedPhaseManager::RequestSpawnCharacter(uint8 TeamID, UTurnBasedCharacterData* InData)
{
	TArray<UTurnBasedCharacterData*>& datas = PendingSpawnMap.FindOrAdd(TeamID);
	datas.Add(InData);
	SpawnRequestCountMap.FindOrAdd(InData->RuntimeData.DataTag)--;

	for (const auto& i : SpawnRequestCountMap)
		if (i.Value > 0)return;
	TrySpawnCharacter();
}