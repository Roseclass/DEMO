#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Datas/GameInstanceTypes.h"
#include "Datas/TurnBasedDataTypes.h"
#include "TurnBasedSubsystem.generated.h"

/**
 * 
 */

class ATurnBasedPhaseManager;
class UPhaseTransitionContext;

UCLASS()
class DEMO_API UTurnBasedSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()
public:
	UTurnBasedSubsystem();
protected:
public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	//property
private:
	ATurnBasedPhaseManager* Manager;
	bool bInitPhaseSystem;
	UPROPERTY()UPhaseTransitionContext* TempContext;
	FPhaseTransitionToken ActiveToken;
	TMap<ETurnBasedFieldId, FTurnBasedFieldLayoutRow*> LevelDatas;
protected:
	UPROPERTY(EditAnywhere)
		TObjectPtr<UTurnBasedSubsystemRegistry> Registry;
public:

	//function
private:
	UFUNCTION()void InitializeTurnBasedField();
	UFUNCTION()void EnterTurnBased(FPhaseTransitionToken InToken, UObject* Context);
	UFUNCTION()void ExitTurnBased(FPhaseTransitionToken InToken);
protected:
public:	
	void InitPhaseSystem();
	FTurnBasedFieldLayoutRow FindSoftByFieldId(ETurnBasedFieldId FieldId) const;


};


/*
* 
* 페이즈 전환시
* 필요한 데이터들을 로드->매니저에 데이터를 담아서 스폰
* 
*/