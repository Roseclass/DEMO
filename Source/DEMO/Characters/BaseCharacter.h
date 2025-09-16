#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "GameplayTagContainer.h"
#include "GenericTeamAgentInterface.h"
#include "AbilitySystemInterface.h"
#include "SaveLoad/Save.h"
#include "BaseCharacter.generated.h"

class UPrimaryDataAsset;
class USpringArmComponent;
class UCameraComponent;
class UAbilityComponent;
class UAttributeSet_Character;

UCLASS()
class DEMO_API ABaseCharacter : public ACharacter, public IGenericTeamAgentInterface, public IAbilitySystemInterface, public ISave
{
	GENERATED_BODY()

public:
	ABaseCharacter();
protected:
	virtual void BeginPlay() override;
public:	
	virtual void Tick(float DeltaTime) override;

	// IGenericTeamAgentInterface interface
	virtual void SetGenericTeamId(const FGenericTeamId& TeamID) final override;
	virtual FGenericTeamId GetGenericTeamId() const override;
	// End of IGenericTeamAgentInterface interface

	// IAbilitySystemInterface interface
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	// End of IAbilitySystemInterface interface

	// ISave interface
	virtual	FGuid GetUniqueSaveName() const final override;
	virtual	void OnBeforeSave(USaveGameData* SaveData) override;
	virtual	void OnAfterLoad(USaveGameData* ReadData) override;
	// End of ISave interface

	//property
private:
	FGuid UniqueSaveName;
	uint8 GenericTeamId = 1;
protected:
	//scene

	//actor
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = GAS, meta = (AllowPrivateAccess = "true"))
		UAbilityComponent* Ability;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = GAS, meta = (AllowPrivateAccess = "true"))
		UAttributeSet_Character* AttributeSet;	
public:

	//function
private:
protected:
public:
	virtual void Init(FGuid NewSaveName,UPrimaryDataAsset* DA);
	virtual FGameplayTag GetDataTag() const;
	virtual TArray<FGameplayTag> GetDataTags() const;
};
