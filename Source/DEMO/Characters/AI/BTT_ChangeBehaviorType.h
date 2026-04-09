#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTT_ChangeBehaviorType.generated.h"

/**
 * 
 */

enum class EBehaviorType : uint8;

UCLASS()
class DEMO_API UBTT_ChangeBehaviorType : public UBTTaskNode
{
	GENERATED_BODY()
public:
	UBTT_ChangeBehaviorType();
protected:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

	//property
private:
protected:
	UPROPERTY(EditAnywhere)
		EBehaviorType BehaviorType;
public:
};
