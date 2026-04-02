#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GameplayTagContainer.h"
#include "TBActiveFXComponent.generated.h"

class UFXSystemComponent;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class DEMO_API UTBActiveFXComponent : public UActorComponent
{
	GENERATED_BODY()
public:
	UTBActiveFXComponent();
protected:
	virtual void BeginPlay() override;
public:
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	//property
private:
	TMap<FName, UFXSystemComponent*> FXMap;
protected:
public:

	//function
private:
protected:
public:
	void RegisterFX(FName RowName, UFXSystemComponent* FX);
	void RemoveFX(FName RowName);

	bool IsActive(FName RowName) const;
	UFXSystemComponent* GetFXFromDataID(FName RowName) const;
};
