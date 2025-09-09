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
	virtual FGenericTeamId GetGenericTeamId() const override;
	// End of IGenericTeamAgentInterface interface

	// IAbilitySystemInterface interface
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	// End of IAbilitySystemInterface interface

	// ISave interface
	virtual	FString GetUniqueSaveName() override;
	virtual	void OnBeforeSave(USaveGameData* SaveData) override;
	virtual	void OnAfterLoad(USaveGameData* ReadData) override;
	// End of ISave interface

	//property
private:
protected:
	//scene

	//actor
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = GAS, meta = (AllowPrivateAccess = "true"))
		UAbilityComponent* Ability;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = GAS, meta = (AllowPrivateAccess = "true"))
		UAttributeSet_Character* AttributeSet;
public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
		USpringArmComponent* CameraBoom;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
		UCameraComponent* FollowCamera;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Input)
		float TurnRateGamepad;

	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = true))
		uint8 TeamID = 1; //enum으로 변경

	//function
private:
protected:
public:
	virtual void Init(UPrimaryDataAsset* DA);
	virtual FGameplayTag GetDataTag() const;
	virtual TArray<FGameplayTag> GetDataTags() const;

	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }

};
