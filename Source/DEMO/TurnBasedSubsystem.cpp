#include "TurnBasedSubsystem.h"
#include "Global.h"

#include "DEMOAssetManager.h"
#include "DEMOGameInstance.h"
#include "SaveLoadSubsystem.h"

#include "Characters/TurnBasedCharacter.h"
#include "Characters/TurnBasedCharacterData.h"

#include "Datas/GameInstanceTypes.h"

#include "Objects/TurnBasedPhaseManager.h"

UTurnBasedSubsystem::UTurnBasedSubsystem()
{
	UTurnBasedSubsystemRegistry* ptr;
	CHelpers::GetAsset<UTurnBasedSubsystemRegistry>(&ptr, "TurnBasedSubsystemRegistry'/Game/Datas/DA_TurnBasedSubsystemRegistry.DA_TurnBasedSubsystemRegistry'");
	Registry = ptr;
}

void UTurnBasedSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	for (auto& i : Registry->CharacterDAMap)
		i.Value->RuntimeData.DataTag = i.Key;

	TArray<FTurnBasedFieldLayoutRow*> datas;
	if (Registry->LevelDT)
	{
		Registry->LevelDT->GetAllRows<FTurnBasedFieldLayoutRow>("", datas);
		for (auto i : datas)
			LevelDatas[i->FieldId] = i;
	}
}

void UTurnBasedSubsystem::InitializeTurnBasedField()
{
	if (Manager)Manager->Destroy();
	CheckNull(TempContext);

	// 위치에 맞게 액터 스폰
	// 이건 월드 서브시스템이나 월드에 배치해둔 콜리전으로 현재 어느 섹션인지 따로 저장해놨다가 받아오자
	LevelDatas;

	Manager = GetWorld()->SpawnActorDeferred<ATurnBasedPhaseManager>(ATurnBasedPhaseManager::StaticClass(), FTransform());
	UGameplayStatics::FinishSpawningActor(Manager, FTransform());

	Manager->AddToSpawnMap(TempContext->Instigator->GetGenericTeamId(), TempContext->Instigator->GetDataTags());
	Manager->AddToSpawnMap(TempContext->Target->GetGenericTeamId(), TempContext->Target->GetDataTags());

	for (auto i : TempContext->Instigator->GetDataTags())
	{
		TArray<FSoftObjectPath> ptrArr;
		ptrArr.Add(Registry->CharacterDAMap[i]->SkeletalMesh.ToSoftObjectPath());
		ptrArr.Add(Registry->CharacterDAMap[i]->AnimBlueprint.ToSoftObjectPath());
		TFunction<void()> bind = [this, i]()
		{
			if (!Manager)
			{
				CLog::Print("???");
				return;
			}
			Manager->RequestSpawnCharacter(TempContext->Instigator->GetGenericTeamId(), Registry->CharacterDAMap[i]);
		};
		UDEMOAssetManager::GetIfValid()->RequestAsyncLoad(ptrArr, MoveTemp(bind));
	}

	for (auto i : TempContext->Target->GetDataTags())
	{
		TArray<FSoftObjectPath> ptrArr;
		ptrArr.Add(Registry->CharacterDAMap[i]->SkeletalMesh.ToSoftObjectPath());
		ptrArr.Add(Registry->CharacterDAMap[i]->AnimBlueprint.ToSoftObjectPath());
		TFunction<void()> bind = [this, i]()
		{
			if (!Manager)
			{
				CLog::Print("???");
				return;
			}
			Manager->RequestSpawnCharacter(TempContext->Target->GetGenericTeamId(), Registry->CharacterDAMap[i]);
		};
		UDEMOAssetManager::GetIfValid()->RequestAsyncLoad(ptrArr, MoveTemp(bind));
	}

	TempContext = nullptr;
}

void UTurnBasedSubsystem::EnterTurnBased(FPhaseTransitionToken InToken, UObject* Context)
{
	ActiveToken = InToken;
	TempContext = Cast<UPhaseTransitionContext>(Context);

	//메인메뉴->TurnBased (비활성화)
	if (ActiveToken.CurrentPhase == EGameInstancePhase::MainMenu)
	{
		//이거는 있을수가없어
	}
	//TPS->TurnBased
	else if (ActiveToken.CurrentPhase == EGameInstancePhase::TPS)
	{
		FLatentActionInfo latentInfo;
		latentInfo.CallbackTarget = this;
		latentInfo.Linkage = 0;
		latentInfo.ExecutionFunction = FName("InitializeTurnBasedField");

		// 레벨데이터에 따라 서브레벨 선택
		// 이건 월드 서브시스템이나 월드에 배치해둔 콜리전으로 현재 어느 섹션인지 따로 저장해놨다가 받아오자

		UGameplayStatics::LoadStreamLevel(GetWorld(), FName(TEXT("DEMO_TB1")), 1, 1, latentInfo);
	}

	UDEMOGameInstance* gi = Cast<UDEMOGameInstance>(GetGameInstance());
	gi->ReportPhaseProgress(ActiveToken);
}

void UTurnBasedSubsystem::ExitTurnBased(FPhaseTransitionToken InToken)
{
	ActiveToken = InToken;

	//TurnBased->메인메뉴
	if (ActiveToken.NextPhase == EGameInstancePhase::MainMenu)
	{
		// 딱히할게없는데?
	}
	//TurnBased->TPS
	else if (ActiveToken.NextPhase == EGameInstancePhase::TPS)
	{
		// 서브레벨 언로드

		// 저장
		USaveLoadSubsystem* SLS = GetGameInstance()->GetSubsystem<USaveLoadSubsystem>();
		SLS->SaveData();
	}

	UDEMOGameInstance* gi = Cast<UDEMOGameInstance>(GetGameInstance());
	gi->ReportPhaseProgress(ActiveToken);
}

void UTurnBasedSubsystem::InitPhaseSystem()
{
	CheckTrue(bInitPhaseSystem);
	bInitPhaseSystem = 1;
	UDEMOGameInstance* gi = Cast<UDEMOGameInstance>(GetGameInstance());
	gi->OnPhaseEnter[int32(EGameInstancePhase::TurnBased)].AddUFunction(this, "EnterTurnBased");
	gi->OnPhaseExit[int32(EGameInstancePhase::TurnBased)].AddUFunction(this, "ExitTurnBased");
}

FTurnBasedFieldLayoutRow UTurnBasedSubsystem::FindSoftByFieldId(ETurnBasedFieldId FieldId) const
{
	return *LevelDatas[FieldId];
}

/*
* 메인메뉴 - > tps 필드 넘어올때 세이브 파일로부터 da 받아서 tps 세팅
* 세팅이 완성되면 노티파이 및 컨트롤 허용(로딩화면추가하기)
* tps 하다가 action으로 오버랩되면 tps필드 멈추고 tb로 넘어감
*/

//TODO:: 매니저들을 통한 애셋 로딩과 da를 통한 캐릭터 세팅