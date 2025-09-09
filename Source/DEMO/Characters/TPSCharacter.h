#pragma once

#include "CoreMinimal.h"
#include "Characters/BaseCharacter.h"
#include "Characters/EventTriggerProvider.h"
#include "Characters/TPSCharacterData.h"
#include "TPSCharacter.generated.h"

/**
 * 
 */

class AEventTrigger;

UCLASS()
class DEMO_API ATPSCharacter : public ABaseCharacter, public IEventTriggerProvider
{
	GENERATED_BODY()
public:
	ATPSCharacter();
protected:
	virtual void BeginPlay() override;
public:
	virtual void Tick(float DeltaTime) override;
	// APawn interface
		virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	// End of APawn interface

	// ISave interface
	virtual	FString GetUniqueSaveName() override;
	virtual	void OnBeforeSave(USaveGameData* SaveData) override;
	virtual	void OnAfterLoad(USaveGameData* ReadData) override;
	// End of ISave interface

	// IEventTriggerProvider interface
	virtual AEventTrigger* GetEventTrigger() override;
	virtual void EnableTriggerCollision(bool bEnable) override;
	// End of IEventTriggerProvider interface

	//property
private:
	FTPSCharacterRuntimeData RuntimeData;
	AEventTrigger* EventTrigger;
protected:

	//scene

	//actor
public:

	//function
private:
protected:
	void Action();
	void MoveForward(float Value);
	void MoveRight(float Value);
public:
	virtual void Init(UPrimaryDataAsset* DA) override;
	virtual FGameplayTag GetDataTag() const override;
	virtual TArray<FGameplayTag> GetDataTags() const override;
};
