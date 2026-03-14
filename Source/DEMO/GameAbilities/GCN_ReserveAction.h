#pragma once

#include "CoreMinimal.h"
#include "GameplayCueNotify_Static.h"
#include "GCN_ReserveAction.generated.h"

/**
 * 
 */

UCLASS(Blueprintable, Category = "GameplayCueNotify")
class DEMO_API UGCN_ReserveAction : public UGameplayCueNotify_Static
{
	GENERATED_BODY()
public:
	UGCN_ReserveAction();
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
