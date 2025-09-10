#pragma once

#include "CoreMinimal.h"
#include "Characters/BaseCharacter.h"
#include "Characters/TurnBasedCharacterData.h"
#include "TurnBasedCharacter.generated.h"

/**
 * 
 */

UCLASS()
class DEMO_API ATurnBasedCharacter : public ABaseCharacter
{
	GENERATED_BODY()
public:
	ATurnBasedCharacter();
protected:
	virtual void BeginPlay() override;
public:
	virtual void Tick(float DeltaTime) override;

	// ISave interface
	virtual	void OnBeforeSave(USaveGameData* SaveData) override;
	virtual	void OnAfterLoad(USaveGameData* ReadData) override;
	// End of ISave interface

	//property
private:
protected:
	UPROPERTY(EditDefaultsOnly)
		UTurnBasedCharacterData* TempDA;

	//scene

	//actor
public:

	//function
private:
protected:
public:
};
