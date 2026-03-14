#pragma once

#include "CoreMinimal.h"
#include "GameplayCueNotify_Static.h"
#include "GCN_ChangeTarget.generated.h"

/**
 * 
 */

UCLASS()
class DEMO_API UGCN_ChangeTarget : public UGameplayCueNotify_Static
{
	GENERATED_BODY()

public:
	UGCN_ChangeTarget();
protected:
public:
	virtual bool OnExecute_Implementation(AActor* MyTarget, const FGameplayCueParameters& Parameters) const override;

	//property
private:
protected:
public:

	//function
private:
protected:
public:
};
