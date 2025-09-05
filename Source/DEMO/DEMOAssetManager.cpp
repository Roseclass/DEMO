#include "DEMOAssetManager.h"
#include "Global.h"

#include "AbilitySystemGlobals.h"

#include "TurnBasedSubsystem.h"

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

UDEMOAssetManager* UDEMOAssetManager::GetIfValid()
{
	UDEMOAssetManager* Singleton = Cast<UDEMOAssetManager>(GEngine->AssetManager);

	if (Singleton)
		return Singleton;

	return nullptr;
}

void UDEMOAssetManager::StartInitialLoading()
{
	Super::StartInitialLoading();
	UAbilitySystemGlobals::Get().InitGlobalData();
}

void UDEMOAssetManager::RequestAsyncLoad(const FSoftObjectPath& TargetToStream, TFunction<void()>&& Callback)
{
	FStreamableManager& sm = GetStreamableManager();
	sm.RequestAsyncLoad(TargetToStream, MoveTemp(Callback));
}

void UDEMOAssetManager::RequestAsyncLoad(TArray<FSoftObjectPath> TargetToStream, TFunction<void()>&& Callback)
{
	FStreamableManager& sm = GetStreamableManager();
	sm.RequestAsyncLoad(TargetToStream, MoveTemp(Callback));
}
