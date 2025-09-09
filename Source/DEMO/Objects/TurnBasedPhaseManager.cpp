#include "Objects/TurnBasedPhaseManager.h"
#include "Global.h"

#include "SaveLoadSubsystem.h"

#include "Characters/TurnBasedCharacter.h"
#include "Characters/TurnBasedCharacterData.h"

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

	TrySpawnCharacter();
}

void ATurnBasedPhaseManager::SpawnCharacter(uint8 TeamID, UTurnBasedCharacterData* InData)
{
	CheckNull(InData);
	ATurnBasedCharacter* ch = GetWorld()->SpawnActorDeferred<ATurnBasedCharacter>(ATurnBasedCharacter::StaticClass(), FTransform());
	UGameplayStatics::FinishSpawningActor(ch, FTransform());
	ch->Init(InData);
	SpawnedCharacterMap.FindOrAdd(TeamID).Add(ch);
}

void ATurnBasedPhaseManager::TrySpawnCharacter()
{
	// 트랜스폼 설정, 서브레벨 dt row도 받아와야함
	for (auto& target : SpawnMap)
	{
		TArray<UTurnBasedCharacterData*>& pending = PendingSpawnMap.FindOrAdd(target.Key);
		while (!pending.IsEmpty())
		{
			UTurnBasedCharacterData* data = pending[pending.Num() - 1];
			pending.RemoveAt(pending.Num() - 1);
			int32 idx = target.Value.Find(data->RuntimeData.DataTag);
			if (idx == INDEX_NONE)continue;
			SpawnCharacter(target.Key, data);
		}
	}
}

void ATurnBasedPhaseManager::AddToSpawnMap(uint8 TeamID, TArray<FGameplayTag> InGameplayTags)
{
	TArray<FGameplayTag>& tags = SpawnMap.FindOrAdd(TeamID);
	tags.Append(InGameplayTags);
}

void ATurnBasedPhaseManager::RequestSpawnCharacter(uint8 TeamID, UTurnBasedCharacterData* InData)
{
	TArray<UTurnBasedCharacterData*>& datas = PendingSpawnMap.FindOrAdd(TeamID);
	datas.Add(InData);
}