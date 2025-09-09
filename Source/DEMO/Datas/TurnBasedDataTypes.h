#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "Engine/DataTable.h"
#include "GameplayTagContainer.h"
#include "TurnBasedDataTypes.generated.h"

/**
 *
 */

//#include "Datas/TurnBasedDataTypes.h"

class UTurnBasedCharacterData;

UENUM(BlueprintType)
enum class ETurnBasedFieldId : uint8
{
    Forest01,
    Desert01,
    Cave01,
    MAX
};

USTRUCT(BlueprintType)
struct FTurnBasedFieldLayoutRow : public FTableRowBase
{
    GENERATED_BODY()

public:
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
        ETurnBasedFieldId FieldId;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
        FName LevelName;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
        FTransform Origin;
};

UCLASS(BlueprintType)
class DEMO_API UTurnBasedSubsystemRegistry : public UDataAsset
{
    GENERATED_BODY()
public:
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
        TMap<FGameplayTag, TObjectPtr<UTurnBasedCharacterData>> CharacterDAMap;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
        TObjectPtr<UDataTable> LevelDT;
};