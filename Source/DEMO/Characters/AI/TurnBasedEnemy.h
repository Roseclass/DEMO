#pragma once

#include "CoreMinimal.h"
#include "Characters/TurnBasedCharacter.h"
#include "TurnBasedEnemy.generated.h"

/**
 * 
 */

class UUW_TBSelect;

DECLARE_DELEGATE_OneParam(FTurnBasedEnemySelectSkill, FGameplayTag);
DECLARE_DELEGATE_OneParam(FTurnBasedEnemySelectTarget, AActor*);

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
	UPROPERTY()UUW_TBSelect* SelectWidget;
protected:
public:
	FTurnBasedEnemySelectSkill OnHoverSkillIcon;
	FTurnBasedEnemySelectSkill OnClickSkillIcon;

	FTurnBasedEnemySelectTarget OnHoverTarget;
	FTurnBasedEnemySelectTarget OnClickTarget;

	//function
private:
protected:
	virtual void InitAssets(UPrimaryDataAsset* DA) override;
	virtual void InitGA(UPrimaryDataAsset* DA) override;
public:
	virtual void Init(FGuid NewSaveName, UPrimaryDataAsset* DA) override;

	void HoverSkillIcon(FGameplayTag InTag);
	void ClickSkillIcon(FGameplayTag InTag);

	void HoverTarget(AActor* InTarget);
	void ClickTarget(AActor* InTarget);

	void LinkSelectWidget(UUW_TBSelect* NewSelectWidget);

};
