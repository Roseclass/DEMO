#pragma once

#include "CoreMinimal.h"
#include "GameplayAbilitySpec.h"
#include "GameplayEffect.h"
#include "AbilityTypes.generated.h"

/*
* 
*/

//#include "GameAbilities/AbilityTypes.h"

USTRUCT(BlueprintType)
struct FAttributeInitialStat
{
    GENERATED_BODY()
public:
    UPROPERTY(EditDefaultsOnly)
        float Stat;

    UPROPERTY(EditDefaultsOnly)
        FGameplayAttribute Attribute;
};

USTRUCT(BlueprintType)
struct FAttributeInitialInfos
{
    GENERATED_BODY()
public:
    UPROPERTY(EditDefaultsOnly)
        TArray<FAttributeInitialStat> InitalStats;
};

USTRUCT(BlueprintType)
struct FAbilitySpecInfo
{
    GENERATED_BODY()
public:
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
        TSubclassOf<UGameplayAbility> AbilityClass = nullptr;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
        int32 Level = 1;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
        int32 InputID = INDEX_NONE;

	UPROPERTY()UObject* SourceObject = nullptr;
};
