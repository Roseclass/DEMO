#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Datas/GameInstanceTypes.h"
#include "Datas/TPSDataTypes.h"
#include "TPSSubsystem.generated.h"

/**
 * 
 */

class ATPSPhaseManager;

UCLASS()
class DEMO_API UTPSSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()
public:
	UTPSSubsystem();
protected:
public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	//property
private:
	ATPSPhaseManager* Manager;
	bool bInitPhaseSystem;
	FPhaseTransitionToken ActiveToken;
	FDelegateHandle LoadMapDelegateHandle;
protected:
	UPROPERTY(EditAnywhere)
		TObjectPtr<UTPSSubsystemRegistry> Registry;
public:

	//function
private:
	void InitializeTPSField();
	UFUNCTION()void EnterTPS(FPhaseTransitionToken InToken, UObject* Context);
	UFUNCTION()void ExitTPS(FPhaseTransitionToken InToken);
protected:
public:
	void InitPhaseSystem();
};
