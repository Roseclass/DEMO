#pragma once

#include "CoreMinimal.h"
#include "GameplayCueNotify_Static.h"
#include "GCN_DamageText.generated.h"

/**
 * 
 */
UCLASS(Blueprintable, Category = "GameplayCueNotify")
class DEMO_API UGCN_DamageText : public UGameplayCueNotify_Static
{
	GENERATED_BODY()
public:
	UGCN_DamageText();
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
