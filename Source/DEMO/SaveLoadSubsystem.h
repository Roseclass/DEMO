#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"

#include "GameplayTagContainer.h"

#include "Engine/DataTable.h"
#include "UObject/NoExportTypes.h"
#include "UObject/WeakInterfacePtr.h"

#include "SaveLoad/SaveLoadTypes.h"

#include "SaveLoadSubsystem.generated.h"

/**
 * 
 */

UCLASS()
class DEMO_API USaveLoadSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()
	//property
private:
	FSaveWriteKey SaveWriteKey;
	UPROPERTY(Transient)USaveGameMetaData* CachedMetaData;
	UPROPERTY(Transient)USaveGameData* CachedGameData;
protected:
public:

	//function
private:
	USaveGameData* CreateDefaultSaveData();
protected:
public:
	// initialize the calss. mustbe called when the first launches
	void Init();

	// Save the current state of the game
	ESaveLoadResult CreateNewData(int32 InSlotIndex, FString InSlotName);

	// Save the current state of the game
	ESaveLoadResult SaveData();

	// Loads the current state of the game
	ESaveLoadResult LoadData(int32 InSlotIndex);

	// Deletes the specified slot
	ESaveLoadResult DeleteData(int32 InSlotIndex);

	FORCEINLINE USaveGameMetaData* ReadMetaData()const { return CachedMetaData; };
	FORCEINLINE USaveGameData* ReadGameData()const { return CachedGameData; };

	FString GetCurrentSaveSlot()const;
	const TArray<FSaveMetaData>& GetAllSaveMetaData()const;
	int32 GetMaxSize()const;
	bool IsEmpty(int32 InSlotIndex)const;
	bool IsSlotNameAvailable(FString InSlotName);
};

/*
* 세션 락: 저장 중엔 두 번째 저장 요청/슬롯 변경/삭제를 막거나 큐잉.
* 배치 경계: 스냅샷 수집과 커밋(I/O)을 함수로 분리(나중에 커밋만 비동기로 바꾸기 쉽게).
*
* 스냅샷 수집(Stage A) 시간이 ~5-8 ms 이하면 OK.
* 파일 쓰기(Stage B)가 50-100 ms 이상으로 체감 히치가 보이면 비동기로 전환.
* 오토세이브를 도입하거나 저장 빈도가 늘어나면 전환 고려.
*
* UI 인디케이터: '저장 중…' 0.2-0.5초 오버레이(심리적 안정).
* 쿼리 최적화: 매번 월드 전역 스캔해도 지금 빈도면 괜찮지만, 약참조 캐시 + 맵 전환 시 재스캔으로 바꿔두면 더 안전.
* 임시->커밋: 가변 슬롯이면 신규 슬롯은 Pending -> Ready 두 단계로(저장 실패 시 흔적 정리 쉬움).
* 테스트: 대형 인벤/월드 상태에서 전투 종료 직후 저장을 3~5회 연속 눌러보고, 프레임 히치가 눈에 띄면 그때 비동기 스위치.
*
*/