#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "Engine/DataTable.h"
#include "GameplayTagContainer.h"
#include "TPSDataTypes.generated.h"

/**
 *
 */

//#include "Datas/TPSDataTypes.h"

class UTPSCharacterData;

UCLASS(BlueprintType)
class DEMO_API UTPSSubsystemRegistry : public UDataAsset
{
    GENERATED_BODY()
public:
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
        TMap<FGameplayTag, TObjectPtr<UTPSCharacterData>> CharacterDAMap;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
        TObjectPtr<UDataTable> LevelDT;
};