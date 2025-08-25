#include "SaveLoad/SaveManager.h"
#include "Global.h"

static const FString kMetadataSaveSlot = "SaveGameMetadata";

TArray<FSaveMetaData> USaveManager::CurrentSavedGamesMetaData;
FString USaveManager::CurrentSaveSlot;
TArray<TWeakInterfacePtr<ISave>> USaveManager::SaveInterfaces;

void USaveManager::Init()
{
	// Make sure the metadata file exists incase the game has never been ran
	USaveGameMetaData* saveMetaData = Cast<USaveGameMetaData>(UGameplayStatics::LoadGameFromSlot(kMetadataSaveSlot, 0));
	if (!saveMetaData)
	{
		// since the metadata file doesn't exist, we need to create one
		saveMetaData = Cast<USaveGameMetaData>(UGameplayStatics::CreateSaveGameObject(USaveGameMetaData::StaticClass()));
		UGameplayStatics::SaveGameToSlot(saveMetaData, kMetadataSaveSlot, 0);
	}
	else
	{
		// find empty slot
		bool flag = 0;
		for (const auto& arr : saveMetaData->SavedGamesMetaData)
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
			int32 idx = saveMetaData->SavedGamesMetaData.Add(FSaveMetaData());
			saveMetaData->SavedGamesMetaData[idx].SlotName = FString();
			saveMetaData->SavedGamesMetaData[idx].Date = FDateTime::Now();
			saveMetaData->SavedGamesMetaData[idx].bIsEmpty = 1;
		}// this is not real data
	}

	// at least 5slots
	while (saveMetaData->SavedGamesMetaData.Num() < 5)
	{
		int32 idx = saveMetaData->SavedGamesMetaData.Add(FSaveMetaData());
		saveMetaData->SavedGamesMetaData[idx].SlotName = FString();
		saveMetaData->SavedGamesMetaData[idx].Date = FDateTime::Now();
		saveMetaData->SavedGamesMetaData[idx].bIsEmpty = 1;
	}

	// save the changes to the metadata file
	UGameplayStatics::SaveGameToSlot(saveMetaData, kMetadataSaveSlot, 0);

	CurrentSavedGamesMetaData = saveMetaData->SavedGamesMetaData;
	CurrentSaveSlot = saveMetaData->ActiveSlot;
}

ESaveLoadResult USaveManager::CreateNewData(int32 InSlotIndex, FString InSlotName)
{
	// check SlotIndex
	if (!CurrentSavedGamesMetaData.IsValidIndex(InSlotIndex))
		return ESaveLoadResult::IndexError;

	if (InSlotName.IsEmpty())
		InSlotName = "DATA" + FString::FromInt(InSlotIndex);

	// delete old Data
	if (UGameplayStatics::DoesSaveGameExist(CurrentSavedGamesMetaData[InSlotIndex].SlotName, 0))
		USaveManager::DeleteData(InSlotIndex);

	// Create a new save game data instace
	USaveGameData* saveGameData = Cast<USaveGameData>(UGameplayStatics::CreateSaveGameObject(USaveGameData::StaticClass()));
	// save the game to the InSlotName slot
	if (!UGameplayStatics::SaveGameToSlot(saveGameData, InSlotName, 0))
		return ESaveLoadResult::Failure_Game;

	// update metaDatas
	CurrentSavedGamesMetaData[InSlotIndex].SlotName = InSlotName;
	CurrentSavedGamesMetaData[InSlotIndex].Date = FDateTime::Now();
	CurrentSavedGamesMetaData[InSlotIndex].bIsEmpty = 0;

	// update the metadata file with the new slot
	USaveGameMetaData* saveMetaData = Cast<USaveGameMetaData>(UGameplayStatics::CreateSaveGameObject(USaveGameMetaData::StaticClass()));
	saveMetaData->SavedGamesMetaData = CurrentSavedGamesMetaData;
	saveMetaData->ActiveSlot = CurrentSaveSlot;
	// save the changes to the metadata file
	if (!UGameplayStatics::SaveGameToSlot(saveMetaData, kMetadataSaveSlot, 0))
		return ESaveLoadResult::Failure_Meta;
	return ESaveLoadResult::Success;
}

ESaveLoadResult USaveManager::SaveData()
{
	// Clear old entries
	SaveInterfaces.Empty();

	// Get all the actors that implement the save interface
	TArray<AActor*>actors;
	UGameplayStatics::GetAllActorsWithInterface(GWorld, USave::StaticClass(), actors);

	for (auto i : actors)
		SaveInterfaces.Add(i);

	// Create a new save game data instace
	USaveGameData* saveGameData = Cast<USaveGameData>(UGameplayStatics::CreateSaveGameObject(USaveGameData::StaticClass()));

	// Go over all the actros that need to be saved and save them
	for (auto& i : SaveInterfaces)
	{
		if (!i.GetObject())continue;

		// let the object know that it's about to be saved
		i->OnBeforeSave(saveGameData);
	}

	// save the game to the current slot
	if(!UGameplayStatics::SaveGameToSlot(saveGameData, CurrentSaveSlot, 0))
		return ESaveLoadResult::Failure_Game;

	// find current slot
	for (int32 i = 0; i < CurrentSavedGamesMetaData.Num(); i++)
	{
		if (CurrentSavedGamesMetaData[i].SlotName != CurrentSaveSlot)continue;
		CurrentSavedGamesMetaData[i].Date = FDateTime::Now();
		CurrentSavedGamesMetaData[i].bIsEmpty = 0;

		// update the metadata file with the new slot
		USaveGameMetaData* saveMetaData = Cast<USaveGameMetaData>(UGameplayStatics::CreateSaveGameObject(USaveGameMetaData::StaticClass()));
		saveMetaData->SavedGamesMetaData = CurrentSavedGamesMetaData;
		saveMetaData->ActiveSlot = CurrentSaveSlot;
		// save the changes to the metadata file
		if(!UGameplayStatics::SaveGameToSlot(saveMetaData, kMetadataSaveSlot, 0))
			return ESaveLoadResult::Failure_Meta;
		return ESaveLoadResult::Success;
	}

	// if its not exist, find empty and create new 
	for (int32 i = 0; i < CurrentSavedGamesMetaData.Num(); i++)
	{
		if (!CurrentSavedGamesMetaData[i].bIsEmpty)continue;
		CurrentSavedGamesMetaData[i].SlotName = CurrentSaveSlot;
		CurrentSavedGamesMetaData[i].Date = FDateTime::Now();
		CurrentSavedGamesMetaData[i].bIsEmpty = 0;

		// update the metadata file with the new slot
		USaveGameMetaData* saveMetaData = Cast<USaveGameMetaData>(UGameplayStatics::CreateSaveGameObject(USaveGameMetaData::StaticClass()));
		saveMetaData->SavedGamesMetaData = CurrentSavedGamesMetaData;
		saveMetaData->ActiveSlot = CurrentSaveSlot;
		// save the changes to the metadata file
		if (!UGameplayStatics::SaveGameToSlot(saveMetaData, kMetadataSaveSlot, 0))
			return ESaveLoadResult::Failure_Meta;
		return ESaveLoadResult::Success;
	}

	// no matched slot, no place to save, Add new slot
	CurrentSavedGamesMetaData.Add(FSaveMetaData());
	CurrentSavedGamesMetaData[CurrentSavedGamesMetaData.Num() - 1].SlotName = CurrentSaveSlot;
	CurrentSavedGamesMetaData[CurrentSavedGamesMetaData.Num() - 1].Date = FDateTime::Now();
	CurrentSavedGamesMetaData[CurrentSavedGamesMetaData.Num() - 1].bIsEmpty = 0;
	
	// update the metadata file with the new slot
	USaveGameMetaData* saveMetaData = Cast<USaveGameMetaData>(UGameplayStatics::CreateSaveGameObject(USaveGameMetaData::StaticClass()));
	saveMetaData->SavedGamesMetaData = CurrentSavedGamesMetaData;
	saveMetaData->ActiveSlot = CurrentSaveSlot;
	// save the changes to the metadata file
	if (!UGameplayStatics::SaveGameToSlot(saveMetaData, kMetadataSaveSlot, 0))
		return ESaveLoadResult::Failure_Meta;
	return ESaveLoadResult::Success;
}

ESaveLoadResult USaveManager::LoadData(int32 InSlotIndex)
{	
	// check SlotIndex
	if (!CurrentSavedGamesMetaData.IsValidIndex(InSlotIndex))
		return ESaveLoadResult::IndexError;

	CurrentSaveSlot = CurrentSavedGamesMetaData[InSlotIndex].SlotName;
	CurrentSavedGamesMetaData[InSlotIndex].Date = FDateTime::Now();
	CurrentSavedGamesMetaData[InSlotIndex].bIsEmpty = 0;

	// update the metadata file with the new slot
	USaveGameMetaData* saveMetaData = Cast<USaveGameMetaData>(UGameplayStatics::CreateSaveGameObject(USaveGameMetaData::StaticClass()));
	saveMetaData->SavedGamesMetaData = CurrentSavedGamesMetaData;
	saveMetaData->ActiveSlot = CurrentSaveSlot;
	// save the changes to the metadata file
	if (!UGameplayStatics::SaveGameToSlot(saveMetaData, kMetadataSaveSlot, 0))
		return ESaveLoadResult::Failure_Meta;

	// start load sequence
	USaveGameData* saveGameData = Cast<USaveGameData>(UGameplayStatics::LoadGameFromSlot(saveMetaData->ActiveSlot, 0));

	if (!saveGameData)
	{
		// Create a new save game data instace
		saveGameData = Cast<USaveGameData>(UGameplayStatics::CreateSaveGameObject(USaveGameData::StaticClass()));
		// save the game to the InSlotName slot
		if (!UGameplayStatics::SaveGameToSlot(saveGameData, saveMetaData->ActiveSlot, 0))
			return ESaveLoadResult::Failure_Game;
		CLog::Print(FString(__FUNCTION__) + " slot" + FString::FromInt(InSlotIndex) + " has no data");
	}

	// Loop over all the actors that need to load data nad load their data
	for (auto& i : SaveInterfaces)
	{
		if (!i.GetObject())continue;
		i->OnAfterLoad(saveGameData);
	}
	return ESaveLoadResult::Success;
}

ESaveLoadResult USaveManager::DeleteData(int32 InSlotIndex)
{
	// check SlotIndex
	if (!CurrentSavedGamesMetaData.IsValidIndex(InSlotIndex))
		return ESaveLoadResult::IndexError;

	FString slotName = CurrentSavedGamesMetaData[InSlotIndex].SlotName;

	// Delete the slot
	if(!UGameplayStatics::DeleteGameInSlot(slotName, 0))
		return ESaveLoadResult::Failure_Game;

	if (CurrentSaveSlot == slotName)
		CurrentSaveSlot = FString();

	CurrentSavedGamesMetaData[InSlotIndex].bIsEmpty = 1;

	// update the metadata file with the new slot
	USaveGameMetaData* saveMetaData = Cast<USaveGameMetaData>(UGameplayStatics::CreateSaveGameObject(USaveGameMetaData::StaticClass()));
	saveMetaData->SavedGamesMetaData = CurrentSavedGamesMetaData;
	saveMetaData->ActiveSlot = CurrentSaveSlot;
	// Save the metadata slot
	if (!UGameplayStatics::SaveGameToSlot(saveMetaData, kMetadataSaveSlot, 0))
		return ESaveLoadResult::Failure_Meta;
	return ESaveLoadResult::Success;
}

void USaveManager::SetCurrentSaveSlot(const FString& slot)
{
	CurrentSaveSlot = slot;
}

FString USaveManager::GetCurrentSaveSlot()
{
	return CurrentSaveSlot;
}

TArray<FSaveMetaData> USaveManager::GetAllSaveMetaData()
{
	return CurrentSavedGamesMetaData;
}

int32 USaveManager::GetMaxSize()
{
	return CurrentSavedGamesMetaData.Num();
}

bool USaveManager::IsEmpty(int32 InSlotIndex)
{
	if (!CurrentSavedGamesMetaData.IsValidIndex(InSlotIndex))
		return 0;
	return CurrentSavedGamesMetaData[InSlotIndex].bIsEmpty;
}
