#include "SaveLoadSubsystem.h"
#include "Global.h"

#include "SaveLoad/Save.h"

static const FString kMetadataSaveSlot = "SaveGameMetadata";

void USaveLoadSubsystem::Init()
{
	SaveWriteKey = FSaveWriteKey();

	// Make sure the metadata file exists incase the game has never been ran
	CachedMetaData = Cast<USaveGameMetaData>(UGameplayStatics::LoadGameFromSlot(kMetadataSaveSlot, 0));
	if (!CachedMetaData)
	{
		// since the metadata file doesn't exist, we need to create one
		CachedMetaData = Cast<USaveGameMetaData>(UGameplayStatics::CreateSaveGameObject(USaveGameMetaData::StaticClass()));
	}
	else
	{
		// find empty slot
		bool flag = 0;
		for (const auto& arr : CachedMetaData->SavedGameMetaDatas)
		{
			if (arr.bIsEmpty)
			{
				flag = 1;
				break;
			}
		}

		// this is no empty slot, make new empty meta slot for UI
		if (!flag)
		{
			int32 idx = CachedMetaData->SavedGameMetaDatas.Add(FSaveMetaData());
			CachedMetaData->SavedGameMetaDatas[idx].SlotName = FString();
			CachedMetaData->SavedGameMetaDatas[idx].Date = FDateTime::Now();
			CachedMetaData->SavedGameMetaDatas[idx].bIsEmpty = 1;
		}// this is not real data
	}

	// at least 5slots
	while (CachedMetaData->SavedGameMetaDatas.Num() < 5)
	{
		int32 idx = CachedMetaData->SavedGameMetaDatas.Add(FSaveMetaData());
		CachedMetaData->SavedGameMetaDatas[idx].SlotName = FString();
		CachedMetaData->SavedGameMetaDatas[idx].Date = FDateTime::Now();
		CachedMetaData->SavedGameMetaDatas[idx].bIsEmpty = 1;
	}

	// save the changes to the metadata file
	UGameplayStatics::SaveGameToSlot(CachedMetaData, kMetadataSaveSlot, 0);
}

ESaveLoadResult USaveLoadSubsystem::CreateNewData(int32 InSlotIndex, FString InSlotName)
{
	// check SlotIndex
	if (!GetAllSaveMetaData().IsValidIndex(InSlotIndex))
		return ESaveLoadResult::IndexError;

	if (InSlotName.IsEmpty())
		InSlotName = "DATA" + FString::FromInt(InSlotIndex);

	// delete old Data
	if (UGameplayStatics::DoesSaveGameExist(GetAllSaveMetaData()[InSlotIndex].SlotName, 0))
		USaveLoadSubsystem::DeleteData(InSlotIndex);

	// Create a new save game data instace
	// save the game to the InSlotName slot
	if (!UGameplayStatics::SaveGameToSlot(USaveGameData::CreateDefaultSaveData(), InSlotName, 0))
		return ESaveLoadResult::Failure_Game;

	// update the metadata file with the new slot
	// update metaDatas
	CachedMetaData->SavedGameMetaDatas[InSlotIndex].SlotName = InSlotName;
	CachedMetaData->SavedGameMetaDatas[InSlotIndex].Date = FDateTime::Now();
	CachedMetaData->SavedGameMetaDatas[InSlotIndex].bIsEmpty = 0;

	// save the changes to the metadata file
	if (!UGameplayStatics::SaveGameToSlot(CachedMetaData, kMetadataSaveSlot, 0))
		return ESaveLoadResult::Failure_Meta;
	return ESaveLoadResult::Success;
}

ESaveLoadResult USaveLoadSubsystem::SaveData()
{
	// Get all the actors that implement the save interface
	TArray<AActor*>actors;
	UGameplayStatics::GetAllActorsWithInterface(GetWorld(), USave::StaticClass(), actors);

	TArray<TWeakInterfacePtr<ISave>> saveInterfaces;
	for (auto i : actors)
	{
		TWeakInterfacePtr<ISave> weak = i;
		saveInterfaces.Add(weak);
	}

	// Create a new save game data instace
	CachedGameData = Cast<USaveGameData>(UGameplayStatics::CreateSaveGameObject(USaveGameData::StaticClass()));

	// Go over all the actros that need to be saved and save them
	for (auto& i : saveInterfaces)
	{
		if (!i.GetObject())continue;

		// let the object know that it's about to be saved
		i->OnBeforeSave(CachedGameData);
	}

	// save the game to the current slot
	FString currentSaveSlot = GetCurrentSaveSlot();
	if (!UGameplayStatics::SaveGameToSlot(CachedGameData, currentSaveSlot, 0))
		return ESaveLoadResult::Failure_Game;

	// find current slot
	const TArray<FSaveMetaData>& saveMetaDatas = GetAllSaveMetaData();
	for (int32 i = 0; i < saveMetaDatas.Num(); i++)
	{
		if (saveMetaDatas[i].SlotName != currentSaveSlot)continue;

		// update the metadata file with the new slot
		CachedMetaData->SavedGameMetaDatas[i].Date = FDateTime::Now();
		CachedMetaData->SavedGameMetaDatas[i].bIsEmpty = 0;
		CachedMetaData->ActiveSlot = currentSaveSlot;

		// save the changes to the metadata file
		if (!UGameplayStatics::SaveGameToSlot(CachedMetaData, kMetadataSaveSlot, 0))
			return ESaveLoadResult::Failure_Meta;
		return ESaveLoadResult::Success;
	}

	// if its not exist, find empty and create new 
	for (int32 i = 0; i < saveMetaDatas.Num(); i++)
	{
		if (!saveMetaDatas[i].bIsEmpty)continue;

		// update the metadata file with the new slot
		CachedMetaData->SavedGameMetaDatas[i].SlotName = currentSaveSlot;
		CachedMetaData->SavedGameMetaDatas[i].Date = FDateTime::Now();
		CachedMetaData->SavedGameMetaDatas[i].bIsEmpty = 0;
		CachedMetaData->ActiveSlot = currentSaveSlot;

		// save the changes to the metadata file
		if (!UGameplayStatics::SaveGameToSlot(CachedMetaData, kMetadataSaveSlot, 0))
			return ESaveLoadResult::Failure_Meta;
		return ESaveLoadResult::Success;
	}

	// no matched slot, no place to save, Add new slot
	CachedMetaData->SavedGameMetaDatas.Add(FSaveMetaData());
	CachedMetaData->SavedGameMetaDatas[CachedMetaData->SavedGameMetaDatas.Num() - 1].SlotName = currentSaveSlot;
	CachedMetaData->SavedGameMetaDatas[CachedMetaData->SavedGameMetaDatas.Num() - 1].Date = FDateTime::Now();
	CachedMetaData->SavedGameMetaDatas[CachedMetaData->SavedGameMetaDatas.Num() - 1].bIsEmpty = 0;
	CachedMetaData->ActiveSlot = currentSaveSlot;

	// save the changes to the metadata file
	if (!UGameplayStatics::SaveGameToSlot(CachedMetaData, kMetadataSaveSlot, 0))
		return ESaveLoadResult::Failure_Meta;
	return ESaveLoadResult::Success;
}

ESaveLoadResult USaveLoadSubsystem::LoadData(int32 InSlotIndex)
{
	// check SlotIndex
	if (!GetAllSaveMetaData().IsValidIndex(InSlotIndex))
		return ESaveLoadResult::IndexError;

	FString currentSaveSlot = GetAllSaveMetaData()[InSlotIndex].SlotName;

	// update the metadata file with the new slot
	CachedMetaData->SavedGameMetaDatas[InSlotIndex].Date = FDateTime::Now();
	CachedMetaData->SavedGameMetaDatas[InSlotIndex].bIsEmpty = 0;
	CachedMetaData->ActiveSlot = currentSaveSlot;

	// save the changes to the metadata file
	if (!UGameplayStatics::SaveGameToSlot(CachedMetaData, kMetadataSaveSlot, 0))
		return ESaveLoadResult::Failure_Meta;

	// start load sequence
	CachedGameData = Cast<USaveGameData>(UGameplayStatics::LoadGameFromSlot(currentSaveSlot, 0));

	if (!CachedGameData)
	{
		// Create a new save game data instace
		CachedGameData = Cast<USaveGameData>(UGameplayStatics::CreateSaveGameObject(USaveGameData::StaticClass()));
		CLog::Print(FString(__FUNCTION__) + " slot" + FString::FromInt(InSlotIndex) + " has no data");
	}

	// save the game to the InSlotName slot
	if (!UGameplayStatics::SaveGameToSlot(CachedGameData, currentSaveSlot, 0))
		return ESaveLoadResult::Failure_Game;

	// Get all the actors that implement the save interface
	TArray<AActor*>actors;
	UGameplayStatics::GetAllActorsWithInterface(GetWorld(), USave::StaticClass(), actors);

	TArray<TWeakInterfacePtr<ISave>> saveInterfaces;
	for (auto i : actors)
	{
		TWeakInterfacePtr<ISave> weak = i;
		saveInterfaces.Add(weak);
	}

	// Loop over all the actors that need to load data nad load their data
	for (auto& i : saveInterfaces)
	{
		if (!i.GetObject())continue;
		i->OnAfterLoad(CachedGameData);
	}
	return ESaveLoadResult::Success;
}

ESaveLoadResult USaveLoadSubsystem::DeleteData(int32 InSlotIndex)
{
	// check SlotIndex
	if (!GetAllSaveMetaData().IsValidIndex(InSlotIndex))
		return ESaveLoadResult::IndexError;

	FString slotName = GetAllSaveMetaData()[InSlotIndex].SlotName;

	// Delete the slot
	if (!UGameplayStatics::DeleteGameInSlot(slotName, 0))
		return ESaveLoadResult::Failure_Game;

	// update the metadata file with the new slot
	CachedMetaData->ActiveSlot = GetCurrentSaveSlot() == slotName ? FString() : GetCurrentSaveSlot();
	CachedMetaData->SavedGameMetaDatas[InSlotIndex].bIsEmpty = 1;

	// Save the metadata slot
	if (!UGameplayStatics::SaveGameToSlot(CachedMetaData, kMetadataSaveSlot, 0))
		return ESaveLoadResult::Failure_Meta;
	return ESaveLoadResult::Success;
}

FString USaveLoadSubsystem::GetCurrentSaveSlot()const
{
	return CachedMetaData ? CachedMetaData->ActiveSlot : FString();
}

const TArray<FSaveMetaData>& USaveLoadSubsystem::GetAllSaveMetaData()const
{
	static const TArray<FSaveMetaData> Empty;
	return CachedMetaData ? CachedMetaData->SavedGameMetaDatas : Empty;
}

int32 USaveLoadSubsystem::GetMaxSize()const
{
	return CachedMetaData ? CachedMetaData->SavedGameMetaDatas.Num() : -1;
}

bool USaveLoadSubsystem::IsEmpty(int32 InSlotIndex)const
{
	if (!CachedMetaData)return 0;
	if (!CachedMetaData->SavedGameMetaDatas.IsValidIndex(InSlotIndex))return 0;
	return CachedMetaData->SavedGameMetaDatas[InSlotIndex].bIsEmpty;
}

bool USaveLoadSubsystem::IsSlotNameAvailable(FString InSlotName)
{
	const TArray<FSaveMetaData>& metaDatas = GetAllSaveMetaData();
	for (auto i : metaDatas)
		if (i.SlotName == InSlotName && !i.bIsEmpty)return 0;
	return 1;
}