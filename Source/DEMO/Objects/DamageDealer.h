// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameplayTagContainer.h"
#include "DamageDealer.generated.h"

class AController;
class UShapeComponent;
class UGameplayEffect;

UENUM(BlueprintType)
enum class EDamageType : uint8
{
	Default,
	Fire,
	Ice,
	Poison,
	Physical,
	Magic
};

UCLASS()
class DEMO_API ADamageDealer : public AActor
{
	GENERATED_BODY()
	
public:	
	ADamageDealer();
protected:
	virtual void BeginPlay() override;
public:	
	virtual void Tick(float DeltaTime) override;

	//property
private:
	bool bAct;
	TArray<UShapeComponent*> CollisionComponents;

	TArray<AActor*> OverlappedActors;
	AActor* CurrentOverlappedActor;

	TArray<AActor*> DamagedActors;
	AActor* CurrentDamagedActor;
protected:
	UPROPERTY(EditDefaultsOnly, Category = "Datas")
		TSubclassOf<UGameplayEffect> GamePlayEffectClass;

	UPROPERTY(EditAnywhere, Category = "Datas")
		uint8 TeamID = 0;

	UPROPERTY(EditDefaultsOnly, Category = "Datas")
		FName OverlapComponentTag = FName("OverlapCollision");

	UPROPERTY(BlueprintReadOnly)
		float DamageAdditive;

	UPROPERTY(BlueprintReadOnly)
		float DamageMultiplicitive;

	UPROPERTY(BlueprintReadOnly)
		AActor* SourceActor;

	UPROPERTY(BlueprintReadOnly)
		AController* InstigatorController;

	UPROPERTY(EditAnywhere, Category = "Lifetime")
		float LifeSpan = 5.0f;

	UPROPERTY(EditAnywhere, Category = "Lifetime")
		bool bAutoDestroy = true;

	UPROPERTY(EditAnywhere, Category = "Effect")
		FGameplayTagContainer DamageEffectTags;

	UPROPERTY(EditAnywhere, Category = "Damage")
		EDamageType DamageType = EDamageType::Default;

	UPROPERTY(EditAnywhere, Category = "Damage")
		float ActivationDelay = 0.0f;

public:

	//function
private:
protected:
	UFUNCTION(BlueprintImplementableEvent)float CalculateDamage(float InPower);
	UFUNCTION()virtual void OnComponentBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()virtual void OnComponentEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
	virtual void ResetDamagedActors();
	virtual void FindCollision();
	virtual void SendDamage(TSubclassOf<UGameplayEffect> EffectClass, AActor* Target, const FHitResult& SweepResult);

	FORCEINLINE const TArray<UShapeComponent*>& GetCollisionComponents()const { return CollisionComponents; }
	FORCEINLINE const TArray<AActor*>& GetOverlappedActors()const { return OverlappedActors; }
	FORCEINLINE const AActor* GetCurrentOverlappedActor()const { return CurrentOverlappedActor; }
	FORCEINLINE const TArray<AActor*>& GetDamagedActors()const { return DamagedActors; }
	FORCEINLINE const AActor* GetCurrentDamagedActor()const { return CurrentDamagedActor; }
public:
	virtual void Activate();
	virtual void Deactivate();

	FORCEINLINE void SetTeamID(uint8 InID) { TeamID = InID; }
	FORCEINLINE void SetDamageAdditive(float InDamageAdditive) { DamageAdditive = InDamageAdditive; }
	FORCEINLINE void SetDamageMultiplicitive(float InDamageMultiplicitive) { DamageMultiplicitive = InDamageMultiplicitive; }
	FORCEINLINE bool IsActivated()const { return bAct; }

};

/*
* 
* 블루프린트 단계에서 추가된 충돌 컴포넌트의 태그에 OverlapComponentTag를 넣어 사용
* 타이머적용
* 
*/