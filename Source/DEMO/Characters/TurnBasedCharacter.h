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

	//property
private:
	FTurnBasedCharacterRuntimeData RuntimeData;
protected:

	//scene

	//actor
public:

	//function
private:
protected:
public:
	virtual void Init(FGuid NewSaveName, UPrimaryDataAsset* DA) override;
};
