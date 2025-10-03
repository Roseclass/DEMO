#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "GameplayTagContainer.h"
#include "DEMOPlayerController.generated.h"

/**
 * 
 */

class UDEMOSkillUIDataRegistry;
class UUW_SkillSlotAssigner;

struct FDEMOSkillUIData;
struct FDEMOSkillUIDatas;

UCLASS()
class DEMO_API ADEMOPlayerController : public APlayerController
{
	GENERATED_BODY()
public:
    ADEMOPlayerController();
private:
protected:
    virtual void BeginPlay() override;
    virtual void SetupInputComponent()override;
public:

    //property
private:
    UPROPERTY()UUW_SkillSlotAssigner* SkillSlotAssigner;
protected:
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
        TObjectPtr<UDEMOSkillUIDataRegistry> UIDataRegistry;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
        TSubclassOf<UUW_SkillSlotAssigner> SkillSlotAssignerClass;

public:

    //function
private:
    UFUNCTION()void ToggleSkillSlotAssigner();
protected:
public:
    const FDEMOSkillUIDatas* FindUIDatasByTag(FGameplayTag InSkillRootTag) const;
    const FDEMOSkillUIData* FindUIDataByTag(FGameplayTag InSkillTag) const;
    UObject* GetEmptyRoundIconResourceObject() const;
};
