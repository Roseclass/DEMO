#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "TurnBasedAIController.generated.h"

/**
 * 
 */

class UBehaviorTree;
class ATurnBasedEnemy;
class UTurnBasedBehaviorComponent;

UCLASS()
class DEMO_API ATurnBasedAIController : public AAIController
{
	GENERATED_BODY()
public:
	ATurnBasedAIController();
protected:
	virtual void BeginPlay() override;
	virtual void OnPossess(APawn* InPawn) override;
	virtual void OnUnPossess() override;
public:
	virtual void Tick(float DeltaTime) override;

	//property
private:
	bool bPossess;
	bool bInit;
	bool bInitBT;
	ATurnBasedEnemy* OwnerEnemy;
	UBehaviorTree* BehaviorTree;
protected:
	UPROPERTY(VisibleDefaultsOnly)
		UTurnBasedBehaviorComponent* Behavior;
public:

	//function
private:
	void InitBT();
protected:
public:
	void Init(UBehaviorTree* InBehaviorTree);
	void StopLogic(FString Reason);
	void RestartLogic();

};
