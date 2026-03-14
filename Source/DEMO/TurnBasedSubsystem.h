#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Datas/GameInstanceTypes.h"
#include "Datas/TurnBasedDataTypes.h"
#include "TurnBasedSubsystem.generated.h"

/**
 * 
 */
struct FCameraMoveEffectContext;
struct FPayloadContext;
struct FEffectEventContext;

class ATurnBasedPhaseManager;
class ATurnBasedEventResolver;
class ATurnBasedCharacter;
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
	ATurnBasedEventResolver* EventResolver;
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

public: // from manager
	void ApplyCameraMove(const FCameraMoveEffectContext* InEffectContext);
	void ReserveAction(const FPayloadContext* InEffectContext);
	void ApplyGE(const FPayloadContext* InEffectContext);
	void SolveHitEvent(const FEffectEventContext* InEffectContext);
	void SolvePreEvent(const FEffectEventContext* InEffectContext);
	void ChangeTarget(ATurnBasedCharacter* InTarget);

	TSet<ATurnBasedCharacter*> GetPlayerCharacters()const;
	TSet<ATurnBasedCharacter*> GetEnemyCharacters()const;
};


/*
* 
* 페이즈 전환시
* 필요한 데이터들을 로드->매니저에 데이터를 담아서 스폰
* 
*/