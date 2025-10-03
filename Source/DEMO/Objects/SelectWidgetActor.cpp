#include "Objects/SelectWidgetActor.h"
#include "Global.h"

#include "Components/WidgetComponent.h"

#include "Objects/TurnBasedPhaseManager.h"

ASelectWidgetActor::ASelectWidgetActor()
{
    CHelpers::CreateComponent<USceneComponent>(this, &Origin, "Origin");
    CHelpers::CreateComponent<UWidgetComponent>(this, &Widget, "Widget", Origin);

	PrimaryActorTick.bCanEverTick = true;
}

ASelectWidgetActor* ASelectWidgetActor::CreateSelectWidgetActor(UWorld* World, TSubclassOf<ASelectWidgetActor> SpawnClass, FTransform SpawnTransform, ATurnBasedPhaseManager* Manager)
{
    CheckNullResult(World, nullptr);

    ASelectWidgetActor* WidgetActor = World->SpawnActorDeferred<ASelectWidgetActor>(
        SpawnClass,
        FTransform(),
        Manager,
        nullptr,
        ESpawnActorCollisionHandlingMethod::AlwaysSpawn
        );
    CheckNullResult(WidgetActor, nullptr);

    WidgetActor->bError = 0;
    UGameplayStatics::FinishSpawningActor(WidgetActor, FTransform());

    return WidgetActor;
}

void ASelectWidgetActor::BeginPlay()
{
	Super::BeginPlay();	
    
    if(bError || !Cast<ATurnBasedPhaseManager>(GetOwner()))
        checkf(false, TEXT("Invalid spawn path for %s"), *GetName());
}

void ASelectWidgetActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

UUserWidget* ASelectWidgetActor::GetWidgetObject()
{
    return Widget->GetWidget();
}

void ASelectWidgetActor::Show()
{
    SetActorHiddenInGame(0);
}

void ASelectWidgetActor::Hide()
{
    SetActorHiddenInGame(1);
}

void ASelectWidgetActor::SetWidgetRelativeTransform(FTransform InTransform)
{
    Widget->SetRelativeTransform(InTransform);
}