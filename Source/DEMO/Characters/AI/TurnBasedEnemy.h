#pragma once

#include "CoreMinimal.h"
#include "Characters/TurnBasedCharacter.h"
#include "TurnBasedEnemy.generated.h"

/**
 * 
 */

class UUW_TurnBased_Select;

DECLARE_DELEGATE(FTurnBasedEnemySignature);

UCLASS()
class DEMO_API ATurnBasedEnemy : public ATurnBasedCharacter
{
	GENERATED_BODY()
public:
	ATurnBasedEnemy();
protected:
	virtual void BeginPlay() override;
public:
	virtual void Tick(float DeltaTime) override;

	//property
private:
	UPROPERTY()UUW_TurnBased_Select* SelectWidget;
protected:
public:
	FTurnBasedEnemySignature OnLeft;
	FTurnBasedEnemySignature OnRight;
	FTurnBasedEnemySignature OnUp;
	FTurnBasedEnemySignature OnDown;
	FTurnBasedEnemySignature OnConfirm;

	//function
private:
protected:
	virtual void InitAssets(UPrimaryDataAsset* DA) override;
	virtual void InitGA(UPrimaryDataAsset* DA) override;
public:
	virtual void Init(FGuid NewSaveName, UPrimaryDataAsset* DA) override;

	void SelectLeft();
	void SelectRight();
	void SelectUp();
	void SelectDown();
	void Confirm();

	void LinkSelectWidget(UUW_TurnBased_Select* NewSelectWidget);

	FGameplayTag GetCurrentSkillTag()const;
	TArray<FGameplayTag> GetAllSkillTags()const;

	ATurnBasedCharacter* GetCurrentTarget()const;
	TArray<ATurnBasedCharacter*> GetTargetArray()const;
	TArray<ATurnBasedCharacter*> GetPlayerArray()const;
	TArray<ATurnBasedCharacter*> GetEnemyArray()const;
};
