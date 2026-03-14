#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTT_SelectTarget.generated.h"

/**
 * 
 */
UCLASS()
class DEMO_API UBTT_SelectTarget : public UBTTaskNode
{
	GENERATED_BODY()
public:
	UBTT_SelectTarget();
protected:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual FString GetStaticDescription() const override;

	//property
private:
protected:
	UPROPERTY(EditAnywhere, Category = "Character")
		FBlackboardKeySelector SelectedCharacter;

	UPROPERTY(EditAnywhere, Category = "Character")
		FBlackboardKeySelector TargetCharacter;
public:

	//function
private:
protected:
public:
};
