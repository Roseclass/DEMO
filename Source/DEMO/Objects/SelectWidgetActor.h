#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SelectWidgetActor.generated.h"

/**
 * 턴제 위젯을 월드에 보여주는 액터
 */

class UUserWidget;
class UWidgetComponent;
class ATurnBasedCharacter;
class ATurnBasedPhaseManager;

DECLARE_MULTICAST_DELEGATE_OneParam(FOnSelectWidgetActorShown, ATurnBasedCharacter*);
DECLARE_MULTICAST_DELEGATE(FOnSelectWidgetActorHidden);


UCLASS()
class DEMO_API ASelectWidgetActor : public AActor
{
	GENERATED_BODY()
public:	
	ASelectWidgetActor();
private:
	friend class ATurnBasedPhaseManager;
	static ASelectWidgetActor* CreateSelectWidgetActor(UWorld* World, TSubclassOf<ASelectWidgetActor> SpawnClass, FTransform SpawnTransform, ATurnBasedPhaseManager* Manager);
protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;

	//property
private:
	bool bError = 1;
protected:
public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		UWidgetComponent* Widget;

	FOnSelectWidgetActorShown OnSelectWidgetActorShown;
	FOnSelectWidgetActorHidden OnSelectWidgetActorHidden;

	//function
private:
protected:
public:
	UUserWidget* GetWidgetObject();
	void Show();
	void Hide();
};
