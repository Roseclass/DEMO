#include "DEMOPlayerState.h"
#include "Global.h"

void ADEMOPlayerState::WriteMetaData(FSaveWriteKey InKey, USaveGameMetaData* InMetaData)
{
	CachedMetaData = InMetaData;
}

void ADEMOPlayerState::WriteGameData(FSaveWriteKey InKey, USaveGameData* InSaveData)
{
	CachedGameData = InSaveData;
}

FString ADEMOPlayerState::GetCurrentSaveSlot()const
{
	return CachedMetaData ? CachedMetaData->ActiveSlot : FString();
}

const TArray<FSaveMetaData>& ADEMOPlayerState::GetAllSaveMetaData()const
{
	static const TArray<FSaveMetaData> Empty;
	return CachedMetaData ? CachedMetaData->SavedGameMetaDatas : Empty;
}

int32 ADEMOPlayerState::GetMaxSize()const
{
	return CachedMetaData ? CachedMetaData->SavedGameMetaDatas.Num() : -1;
}

bool ADEMOPlayerState::IsEmpty(int32 InSlotIndex)const
{
	if (!CachedMetaData)return 0;
	if (!CachedMetaData->SavedGameMetaDatas.IsValidIndex(InSlotIndex))return 0;
	return CachedMetaData->SavedGameMetaDatas[InSlotIndex].bIsEmpty;
}