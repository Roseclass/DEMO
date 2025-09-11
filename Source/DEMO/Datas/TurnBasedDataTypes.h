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
    TEST,
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

    UPROPERTY(EditAnywhere)
        FVector PlayerStart;

    UPROPERTY(EditAnywhere)
        FVector PlayerEnd;

	UPROPERTY(EditAnywhere, meta = (DisplayName = "PlayerRotation"))
        FQuat PlayerRotation;

    UPROPERTY(EditAnywhere)
        FVector EnemyStart;

    UPROPERTY(EditAnywhere)
        FVector EnemyEnd;

	UPROPERTY(EditAnywhere, meta = (DisplayName = "EnemyRotation"))
        FQuat EnemyRotation;
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