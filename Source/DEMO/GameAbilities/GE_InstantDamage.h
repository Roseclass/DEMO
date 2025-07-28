#pragma once

#include "CoreMinimal.h"
#include "GameplayEffect.h"
#include "GE_InstantDamage.generated.h"

/**
 * 
 */

UCLASS()
class DEMO_API UGE_InstantDamage : public UGameplayEffect
{
	GENERATED_BODY()
public:
    UGE_InstantDamage();
protected:
public:

    //property
private:
protected:
public:

    //function
private:
protected:
public:
};

/*
* 즉발형 데미지를 넣는 GE
* 데미지 적용시 GCN_DamageText를 트리거함
*/