#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "SaveLoad/SaveLoadTypes.h"
#include "DEMOPlayerState.generated.h"

/**
 * 
 */

UCLASS()
class DEMO_API ADEMOPlayerState : public APlayerState
{
	GENERATED_BODY()
	//property
private:
	UPROPERTY(Transient)
		USaveGameMetaData* CachedMetaData;

	UPROPERTY(Transient)
		USaveGameData* CachedGameData;
protected:
public:

	//function
private:
protected:
public:
	void WriteMetaData(FSaveWriteKey InKey, USaveGameMetaData* InMetaData);
	void WriteGameData(FSaveWriteKey InKey, USaveGameData* InSaveData);

	FORCEINLINE USaveGameMetaData* ReadMetaData()const { return CachedMetaData; };
	FORCEINLINE USaveGameData* ReadGameData()const { return CachedGameData; };

	FString GetCurrentSaveSlot()const;
	const TArray<FSaveMetaData>& GetAllSaveMetaData()const;
	int32 GetMaxSize()const;
	bool IsEmpty(int32 InSlotIndex)const;
};

//
// 캐릭터들의 상태를 저장
// 스테이지 진행도 저장
//