#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GameplayTagContainer.h"
#include "TurnBasedCameraComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class DEMO_API UTurnBasedCameraComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UTurnBasedCameraComponent();
protected:
	virtual void BeginPlay() override;
public:	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;

	//property
private:
protected:
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
		FGameplayTag SkillRootTag;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
		TMap<FGameplayTag, FTransform> SkillCameraTransform;
public:

	//function
private:
protected:
public:

};
