#pragma once

#include "CoreMinimal.h"
#include "GameplayCueNotify_Static.h"
#include "GCN_SpawnDamageDealer.generated.h"

/**
 * 
 */

UCLASS(Blueprintable, Category = "GameplayCueNotify")
class DEMO_API UGCN_SpawnDamageDealer : public UGameplayCueNotify_Static
{
	GENERATED_BODY()
public:
	UGCN_SpawnDamageDealer();
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
