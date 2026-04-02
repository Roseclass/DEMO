#include "Characters/TBActiveFXComponent.h"
#include "Global.h"
#include "Particles/ParticleSystemComponent.h"

UTBActiveFXComponent::UTBActiveFXComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UTBActiveFXComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UTBActiveFXComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void UTBActiveFXComponent::RegisterFX(FName RowName, UFXSystemComponent* FX)
{
	CheckTrue_Print(FXMap.Contains(RowName) && FXMap[RowName], " already registered!!");
	CheckTrue_Print(!FX, " fx is nullptr!!");
	FXMap.FindOrAdd(RowName) = FX;
}

void UTBActiveFXComponent::RemoveFX(FName RowName)
{
	CheckTrue_Print(!FXMap.FindOrAdd(RowName), " already removed!!");

	FXMap[RowName]->DestroyComponent();
	FXMap[RowName] = nullptr;
}

bool UTBActiveFXComponent::IsActive(FName RowName) const
{
	if(FXMap.Contains(RowName))
		return FXMap.Find(RowName) != nullptr;
	return 0;
}

UFXSystemComponent* UTBActiveFXComponent::GetFXFromDataID(FName RowName) const
{
	if(FXMap.Contains(RowName))return FXMap[RowName];
	return nullptr;
}
