#pragma once

#include "CoreMinimal.h"
#include "Objects/DamageDealer.h"
#include "Projectile.generated.h"

/**
 * 
 */

class UProjectileMovementComponent;

UCLASS()
class DEMO_API AProjectile : public ADamageDealer
{
	GENERATED_BODY()

public:
	AProjectile();
protected:
	virtual void BeginPlay() override;
public:
	virtual void Tick(float DeltaTime) override;

	//property
private:
	bool bCollsion;
	bool bCollsionEvent;
	FHitResult HitResult;
	float CurrentDamageTick;
	TArray<UShapeComponent*> CollisionComponents;
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		UProjectileMovementComponent* ProjectileMovementComponent;

	UPROPERTY(EditDefaultsOnly, Category = "Datas|Init")
		FName OverlapComponentTag = FName("OverlapCollision");

	UPROPERTY(EditDefaultsOnly, Category = "Datas|Sequence", meta = (ClampMin = 0.10))
		float DamageTick = 0.1;

public:

	//function
private:
protected:
	UFUNCTION()void OnComponentBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()void OnComponentEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
	void FindCollision();

	virtual bool TryDestroy() override;

	FORCEINLINE const TArray<UShapeComponent*>& GetCollisionComponents()const { return CollisionComponents; }
public:

	UFUNCTION(BlueprintCallable, BlueprintPure)AActor* GetTarget();
};
