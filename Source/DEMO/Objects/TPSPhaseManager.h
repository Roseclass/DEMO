#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TPSPhaseManager.generated.h"

class ATPSCharacter;
class UTPSCharacterData;

UCLASS()
class DEMO_API ATPSPhaseManager : public AActor
{
	GENERATED_BODY()
	
public:	
	ATPSPhaseManager();
protected:
	virtual void BeginPlay() override;
public:	
	virtual void Tick(float DeltaTime) override;

	//property
private:
	TSet<ATPSCharacter*> CharacterSet;
	TArray<TTuple<FGuid, UTPSCharacterData*>> SpawnQueue;
protected:
public:

	//function
private:
	void SpawnCharacter(FGuid InSaveName, UTPSCharacterData* InData);
	void TrySpawnCharacter();
protected:
public:
	void InitPlayerCharacter(FGuid InSaveName, UTPSCharacterData* InData);
	void RequestLoadCharacter(FGuid InSaveName, UTPSCharacterData* InData);
	void RequestSpawnCharacter(UTPSCharacterData* InData);
};
