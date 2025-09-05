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
	TQueue<UTPSCharacterData*> SpawnQueue;
	TQueue<UTPSCharacterData*> LoadQueue;
protected:
public:

	//function
private:
	void SpawnCharacter(UTPSCharacterData* InData, bool bLoad = 0);
	void TrySpawnCharacter(TQueue<UTPSCharacterData*>& InQueue, bool bLoad = 0);
protected:
public:
	void RequestSpawnCharacter(UTPSCharacterData* InData);
	void RequestLoadCharacter(UTPSCharacterData* InData);
};
