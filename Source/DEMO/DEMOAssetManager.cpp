#include "DEMOAssetManager.h"
#include "AbilitySystemGlobals.h"

UDEMOAssetManager& UDEMOAssetManager::Get()
{
	UDEMOAssetManager* Singleton = Cast<UDEMOAssetManager>(GEngine->AssetManager);

	if (Singleton)
	{
		return *Singleton;
	}
	else
	{
		UE_LOG(LogTemp, Fatal, TEXT("Invalid AssetManager in DefaultEngine.ini, must be DEMOAssetManager!"));
		return *NewObject<UDEMOAssetManager>();	 // never calls this
	}
}


void UDEMOAssetManager::StartInitialLoading()
{
	Super::StartInitialLoading();
	UAbilitySystemGlobals::Get().InitGlobalData();
}
