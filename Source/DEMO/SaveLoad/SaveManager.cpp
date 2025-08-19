#include "SaveLoad/SaveManager.h"
#include "Global.h"

static const FString kMetadataSaveSlot = "SaveGameMetadata";

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
	}
	else
	{
		TMap<int32, const FSaveMetaData* > orderMap;
		for (const auto& tuple : saveMetaData->SavedGamesMetaData)
			orderMap.Add(tuple.Value.Order, &tuple.Value);

		int32 idx = 0;
		for (const auto& tuple : orderMap)
			saveMetaData->SavedGamesMetaData.FindOrAdd(tuple.Value->SlotName).Order = idx++;
	}
	UGameplayStatics::SaveGameToSlot(saveMetaData, kMetadataSaveSlot, 0);

	CurrentSaveSlot = saveMetaData->ActiveSlot;
}

FString USaveManager::CreateNewData(FString InSlotName)
{
	// update the metadata file with the new slot
	USaveGameMetaData* saveMetaData = Cast<USaveGameMetaData>(UGameplayStatics::LoadGameFromSlot(kMetadataSaveSlot, 0));

	// check NewSlotName
	if (InSlotName.IsEmpty())
		InSlotName = "Slot" + FString::FromInt(saveMetaData->SavedGamesMetaData.Num());

	// Create a new save game data instace
	USaveGameData* saveGameData = Cast<USaveGameData>(UGameplayStatics::CreateSaveGameObject(USaveGameData::StaticClass()));

	// save the game to the curren slot
	UGameplayStatics::SaveGameToSlot(saveGameData, InSlotName, 0);

	FSaveMetaData& saveMetadata = saveMetaData->SavedGamesMetaData.FindOrAdd(InSlotName);
	saveMetadata.SlotName = InSlotName;
	saveMetadata.Date = FDateTime::Now();

	// save the changes to the metadata file
	UGameplayStatics::SaveGameToSlot(saveMetaData, kMetadataSaveSlot, 0);

	return InSlotName;
}

void USaveManager::SaveData()
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
	UGameplayStatics::SaveGameToSlot(saveGameData, CurrentSaveSlot, 0);

	// update the metadata file with the new slot
	USaveGameMetaData* saveMetaData = Cast<USaveGameMetaData>(UGameplayStatics::LoadGameFromSlot(kMetadataSaveSlot, 0));

	FSaveMetaData& saveMetadata = saveMetaData->SavedGamesMetaData.FindOrAdd(CurrentSaveSlot);
	saveMetadata.SlotName = CurrentSaveSlot;
	saveMetadata.Date = FDateTime::Now();

	// save the changes to the metadata file
	UGameplayStatics::SaveGameToSlot(saveMetaData, kMetadataSaveSlot, 0);
}

void USaveManager::LoadData(FString InSlotName)
{
	if (InSlotName.IsEmpty())
		InSlotName = CurrentSaveSlot;

	USaveGameData* saveGameData = Cast<USaveGameData>(UGameplayStatics::LoadGameFromSlot(InSlotName, 0));

	if (!saveGameData)
	{
		// no saves exist yet for this slot. save a default one.
		InSlotName = CreateNewData(FString());

		// Reload it
		saveGameData = Cast<USaveGameData>(UGameplayStatics::LoadGameFromSlot(CurrentSaveSlot, 0));
	}

	// Loop over all the actors that need to load data nad load their data
	for (auto& i : SaveInterfaces)
	{
		if (!i.GetObject())continue;
		i->OnAfterLoad(saveGameData);
	}

	CurrentSaveSlot = InSlotName;

	// update the metadata file with the new slot
	USaveGameMetaData* saveMetaData = Cast<USaveGameMetaData>(UGameplayStatics::LoadGameFromSlot(kMetadataSaveSlot, 0));
	saveMetaData->ActiveSlot = InSlotName;

	FSaveMetaData& saveMetadata = saveMetaData->SavedGamesMetaData.FindOrAdd(CurrentSaveSlot);
	saveMetadata.SlotName = CurrentSaveSlot;
	saveMetadata.Date = FDateTime::Now();

	// save the changes to the metadata file
	UGameplayStatics::SaveGameToSlot(saveMetaData, kMetadataSaveSlot, 0);
}

void USaveManager::DeleteData(const FString SlotName)
{
	// Delete the slot
	UGameplayStatics::DeleteGameInSlot(SlotName, 0);

	if (CurrentSaveSlot == SlotName)
		CurrentSaveSlot = FString();

	// Loading the metadata file
	USaveGameMetaData* saveMetaData = Cast<USaveGameMetaData>(UGameplayStatics::LoadGameFromSlot(kMetadataSaveSlot, 0));

	saveMetaData->SavedGamesMetaData.Remove(SlotName);
	if (saveMetaData->ActiveSlot == SlotName)
		saveMetaData->ActiveSlot = FString();

	// Save the metadata slot
	UGameplayStatics::SaveGameToSlot(saveMetaData, kMetadataSaveSlot, 0);
}

void USaveManager::ActivateData()
{
	// update the metadata file with the new slot
	USaveGameMetaData* saveMetaData = Cast<USaveGameMetaData>(UGameplayStatics::LoadGameFromSlot(kMetadataSaveSlot, 0));

	FSaveMetaData& saveMetadata = saveMetaData->SavedGamesMetaData.FindOrAdd(CurrentSaveSlot);
	saveMetadata.SlotName = CurrentSaveSlot;
	saveMetadata.Date = FDateTime::Now();
	saveMetaData->ActiveSlot = CurrentSaveSlot;;

	// save the changes to the metadata file
	UGameplayStatics::SaveGameToSlot(saveMetaData, kMetadataSaveSlot, 0);
}

void USaveManager::ResetData()
{
	// Create a new save game data instace
	USaveGameData* saveGameData = Cast<USaveGameData>(UGameplayStatics::CreateSaveGameObject(USaveGameData::StaticClass()));

	// save the game to the curren slot
	UGameplayStatics::SaveGameToSlot(saveGameData, CurrentSaveSlot, 0);

	// update the metadata file with the new slot
	USaveGameMetaData* saveMetaData = Cast<USaveGameMetaData>(UGameplayStatics::LoadGameFromSlot(kMetadataSaveSlot, 0));

	FSaveMetaData& saveMetadata = saveMetaData->SavedGamesMetaData.FindOrAdd(CurrentSaveSlot);
	saveMetadata.SlotName = CurrentSaveSlot;
	saveMetadata.Date = FDateTime::Now();

	if(saveMetaData->ActiveSlot == CurrentSaveSlot) 
		saveMetaData->ActiveSlot = FString();

	// save the changes to the metadata file
	UGameplayStatics::SaveGameToSlot(saveMetaData, kMetadataSaveSlot, 0);
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
	TArray<FSaveMetaData> metadata;

	// Loading the metadata file
	USaveGameMetaData* saveMetaData = Cast<USaveGameMetaData>(UGameplayStatics::LoadGameFromSlot(kMetadataSaveSlot, 0));

	metadata.Reserve(saveMetaData->SavedGamesMetaData.Num());

	// Add each saved game's metadat to the return array
	for (const auto& i : saveMetaData->SavedGamesMetaData)
	{
		metadata.Push(i.Value);
	}

	return metadata;
}

int32 USaveManager::GetMaxSize()
{
	USaveGameMetaData* saveMetaData = Cast<USaveGameMetaData>(UGameplayStatics::LoadGameFromSlot(kMetadataSaveSlot, 0));
	if (!saveMetaData)return 0;
	return saveMetaData->SavedGamesMetaData.Num();
}

bool USaveManager::IsActivate(const FString& slot)
{
	// Loading the metadata file
	USaveGameMetaData* saveMetaData = Cast<USaveGameMetaData>(UGameplayStatics::LoadGameFromSlot(kMetadataSaveSlot, 0));
	if (!saveMetaData)return 0;
	return saveMetaData->ActiveSlot == slot;
}
