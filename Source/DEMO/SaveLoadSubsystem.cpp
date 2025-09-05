#include "SaveLoadSubsystem.h"
#include "Global.h"

#include "DEMOPlayerState.h"

static const FString kMetadataSaveSlot = "SaveGameMetadata";

USaveGameData* USaveLoadSubsystem::CreateDefaultSaveData()
{
	USaveGameData* saveGameData = Cast<USaveGameData>(UGameplayStatics::CreateSaveGameObject(USaveGameData::StaticClass()));

	saveGameData->SavedPlayerDatas;
	saveGameData->SavedPlayerDatas.Add(FSaveData());
	saveGameData->SavedPlayerDatas[0].DATag = FGameplayTag::RequestGameplayTag("Data.Terra");
	
	saveGameData->SavedEnemyDatas;

	return saveGameData;
}

void USaveLoadSubsystem::Init()
{
	SaveWriteKey = FSaveWriteKey();
	PS = Cast<ADEMOPlayerState>(UGameplayStatics::GetPlayerState(GetWorld(), 0));

	CheckTrue_Print(!PS, "PS cast Failed!!");

	// Make sure the metadata file exists incase the game has never been ran
	USaveGameMetaData* saveMetaData = Cast<USaveGameMetaData>(UGameplayStatics::LoadGameFromSlot(kMetadataSaveSlot, 0));
	if (!saveMetaData)
	{
		// since the metadata file doesn't exist, we need to create one
		saveMetaData = Cast<USaveGameMetaData>(UGameplayStatics::CreateSaveGameObject(USaveGameMetaData::StaticClass()));
	}
	else
	{
		// find empty slot
		bool flag = 0;
		for (const auto& arr : saveMetaData->SavedGameMetaDatas)
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
			int32 idx = saveMetaData->SavedGameMetaDatas.Add(FSaveMetaData());
			saveMetaData->SavedGameMetaDatas[idx].SlotName = FString();
			saveMetaData->SavedGameMetaDatas[idx].Date = FDateTime::Now();
			saveMetaData->SavedGameMetaDatas[idx].bIsEmpty = 1;
		}// this is not real data
	}

	// at least 5slots
	while (saveMetaData->SavedGameMetaDatas.Num() < 5)
	{
		int32 idx = saveMetaData->SavedGameMetaDatas.Add(FSaveMetaData());
		saveMetaData->SavedGameMetaDatas[idx].SlotName = FString();
		saveMetaData->SavedGameMetaDatas[idx].Date = FDateTime::Now();
		saveMetaData->SavedGameMetaDatas[idx].bIsEmpty = 1;
	}

	// save the changes to the metadata file
	PS->WriteMetaData(SaveWriteKey, saveMetaData);
	UGameplayStatics::SaveGameToSlot(PS->ReadMetaData(), kMetadataSaveSlot, 0);
}

ESaveLoadResult USaveLoadSubsystem::CreateNewData(int32 InSlotIndex, FString InSlotName)
{
	// check SlotIndex
	if (!PS->GetAllSaveMetaData().IsValidIndex(InSlotIndex))
		return ESaveLoadResult::IndexError;

	if (InSlotName.IsEmpty())
		InSlotName = "DATA" + FString::FromInt(InSlotIndex);

	// delete old Data
	if (UGameplayStatics::DoesSaveGameExist(PS->GetAllSaveMetaData()[InSlotIndex].SlotName, 0))
		USaveLoadSubsystem::DeleteData(InSlotIndex);

	// Create a new save game data instace
	// save the game to the InSlotName slot
	PS->WriteGameData(SaveWriteKey, CreateDefaultSaveData());
	if (!UGameplayStatics::SaveGameToSlot(PS->ReadGameData(), InSlotName, 0))
		return ESaveLoadResult::Failure_Game;

	// update the metadata file with the new slot
	USaveGameMetaData* saveMetaData = Cast<USaveGameMetaData>(UGameplayStatics::CreateSaveGameObject(USaveGameMetaData::StaticClass()));

	// update metaDatas
	saveMetaData->SavedGameMetaDatas = PS->GetAllSaveMetaData();
	saveMetaData->SavedGameMetaDatas[InSlotIndex].SlotName = InSlotName;
	saveMetaData->SavedGameMetaDatas[InSlotIndex].Date = FDateTime::Now();
	saveMetaData->SavedGameMetaDatas[InSlotIndex].bIsEmpty = 0;
	saveMetaData->ActiveSlot = PS->GetCurrentSaveSlot();

	// save the changes to the metadata file
	PS->WriteMetaData(SaveWriteKey, saveMetaData);
	if (!UGameplayStatics::SaveGameToSlot(PS->ReadMetaData(), kMetadataSaveSlot, 0))
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
	USaveGameData* saveGameData = Cast<USaveGameData>(UGameplayStatics::CreateSaveGameObject(USaveGameData::StaticClass()));

	// Go over all the actros that need to be saved and save them
	for (auto& i : saveInterfaces)
	{
		if (!i.GetObject())continue;

		// let the object know that it's about to be saved
		i->OnBeforeSave(saveGameData);
	}

	// save the game to the current slot
	FString currentSaveSlot = PS->GetCurrentSaveSlot();
	PS->WriteGameData(SaveWriteKey, saveGameData);
	if (!UGameplayStatics::SaveGameToSlot(PS->ReadGameData(), currentSaveSlot, 0))
		return ESaveLoadResult::Failure_Game;

	// find current slot
	const TArray<FSaveMetaData>& saveMetaDatas = PS->GetAllSaveMetaData();
	for (int32 i = 0; i < saveMetaDatas.Num(); i++)
	{
		if (saveMetaDatas[i].SlotName != currentSaveSlot)continue;

		// update the metadata file with the new slot
		USaveGameMetaData* saveMetaData = Cast<USaveGameMetaData>(UGameplayStatics::CreateSaveGameObject(USaveGameMetaData::StaticClass()));
		saveMetaData->SavedGameMetaDatas = saveMetaDatas;
		saveMetaData->SavedGameMetaDatas[i].Date = FDateTime::Now();
		saveMetaData->SavedGameMetaDatas[i].bIsEmpty = 0;
		saveMetaData->ActiveSlot = currentSaveSlot;

		// save the changes to the metadata file
		PS->WriteMetaData(SaveWriteKey, saveMetaData);
		if (!UGameplayStatics::SaveGameToSlot(PS->ReadMetaData(), kMetadataSaveSlot, 0))
			return ESaveLoadResult::Failure_Meta;
		return ESaveLoadResult::Success;
	}

	// if its not exist, find empty and create new 
	for (int32 i = 0; i < saveMetaDatas.Num(); i++)
	{
		if (!saveMetaDatas[i].bIsEmpty)continue;

		// update the metadata file with the new slot
		USaveGameMetaData* saveMetaData = Cast<USaveGameMetaData>(UGameplayStatics::CreateSaveGameObject(USaveGameMetaData::StaticClass()));
		saveMetaData->SavedGameMetaDatas = saveMetaDatas;
		saveMetaData->SavedGameMetaDatas[i].SlotName = currentSaveSlot;
		saveMetaData->SavedGameMetaDatas[i].Date = FDateTime::Now();
		saveMetaData->SavedGameMetaDatas[i].bIsEmpty = 0;
		saveMetaData->ActiveSlot = currentSaveSlot;

		// save the changes to the metadata file
		PS->WriteMetaData(SaveWriteKey, saveMetaData);
		if (!UGameplayStatics::SaveGameToSlot(PS->ReadMetaData(), kMetadataSaveSlot, 0))
			return ESaveLoadResult::Failure_Meta;
		return ESaveLoadResult::Success;
	}

	// no matched slot, no place to save, Add new slot
	USaveGameMetaData* saveMetaData = Cast<USaveGameMetaData>(UGameplayStatics::CreateSaveGameObject(USaveGameMetaData::StaticClass()));
	saveMetaData->SavedGameMetaDatas = saveMetaDatas;
	saveMetaData->SavedGameMetaDatas.Add(FSaveMetaData());
	saveMetaData->SavedGameMetaDatas[saveMetaData->SavedGameMetaDatas.Num() - 1].SlotName = currentSaveSlot;
	saveMetaData->SavedGameMetaDatas[saveMetaData->SavedGameMetaDatas.Num() - 1].Date = FDateTime::Now();
	saveMetaData->SavedGameMetaDatas[saveMetaData->SavedGameMetaDatas.Num() - 1].bIsEmpty = 0;
	saveMetaData->ActiveSlot = currentSaveSlot;

	// save the changes to the metadata file
	PS->WriteMetaData(SaveWriteKey, saveMetaData);
	if (!UGameplayStatics::SaveGameToSlot(PS->ReadMetaData(), kMetadataSaveSlot, 0))
		return ESaveLoadResult::Failure_Meta;
	return ESaveLoadResult::Success;
}

ESaveLoadResult USaveLoadSubsystem::LoadData(int32 InSlotIndex)
{
	const TArray<FSaveMetaData>& saveMetaDatas = PS->GetAllSaveMetaData();

	// check SlotIndex
	if (!saveMetaDatas.IsValidIndex(InSlotIndex))
		return ESaveLoadResult::IndexError;

	FString currentSaveSlot = saveMetaDatas[InSlotIndex].SlotName;

	// update the metadata file with the new slot
	USaveGameMetaData* saveMetaData = Cast<USaveGameMetaData>(UGameplayStatics::CreateSaveGameObject(USaveGameMetaData::StaticClass()));
	saveMetaData->SavedGameMetaDatas = saveMetaDatas;
	saveMetaData->SavedGameMetaDatas[InSlotIndex].Date = FDateTime::Now();
	saveMetaData->SavedGameMetaDatas[InSlotIndex].bIsEmpty = 0;
	saveMetaData->ActiveSlot = currentSaveSlot;

	// save the changes to the metadata file
	PS->WriteMetaData(SaveWriteKey, saveMetaData);
	if (!UGameplayStatics::SaveGameToSlot(PS->ReadMetaData(), kMetadataSaveSlot, 0))
		return ESaveLoadResult::Failure_Meta;

	// start load sequence
	USaveGameData* saveGameData = Cast<USaveGameData>(UGameplayStatics::LoadGameFromSlot(saveMetaData->ActiveSlot, 0));

	if (!saveGameData)
	{
		// Create a new save game data instace
		saveGameData = Cast<USaveGameData>(UGameplayStatics::CreateSaveGameObject(USaveGameData::StaticClass()));
		CLog::Print(FString(__FUNCTION__) + " slot" + FString::FromInt(InSlotIndex) + " has no data");
	}

	// save the game to the InSlotName slot
	PS->WriteGameData(SaveWriteKey, saveGameData);
	if (!UGameplayStatics::SaveGameToSlot(PS->ReadGameData(), currentSaveSlot, 0))
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
		i->OnAfterLoad(PS->ReadGameData());
	}
	return ESaveLoadResult::Success;
}

ESaveLoadResult USaveLoadSubsystem::DeleteData(int32 InSlotIndex)
{
	const TArray<FSaveMetaData>& saveMetaDatas = PS->GetAllSaveMetaData();

	// check SlotIndex
	if (!saveMetaDatas.IsValidIndex(InSlotIndex))
		return ESaveLoadResult::IndexError;

	FString slotName = saveMetaDatas[InSlotIndex].SlotName;

	// Delete the slot
	if (!UGameplayStatics::DeleteGameInSlot(slotName, 0))
		return ESaveLoadResult::Failure_Game;

	// update the metadata file with the new slot
	USaveGameMetaData* saveMetaData = Cast<USaveGameMetaData>(UGameplayStatics::CreateSaveGameObject(USaveGameMetaData::StaticClass()));
	saveMetaData->SavedGameMetaDatas = saveMetaDatas;
	saveMetaData->ActiveSlot = PS->GetCurrentSaveSlot() == slotName ? FString() : PS->GetCurrentSaveSlot();
	saveMetaData->SavedGameMetaDatas[InSlotIndex].bIsEmpty = 1;

	// Save the metadata slot
	PS->WriteMetaData(SaveWriteKey, saveMetaData);
	if (!UGameplayStatics::SaveGameToSlot(PS->ReadMetaData(), kMetadataSaveSlot, 0))
		return ESaveLoadResult::Failure_Meta;
	return ESaveLoadResult::Success;
}