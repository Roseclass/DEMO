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
	TArray<UShapeComponent*> CollisionComponents;
protected:
	UPROPERTY(EditDefaultsOnly, Category = "Datas|Sequence")
		bool bUseCollsionEvent = 1;

	UPROPERTY(EditDefaultsOnly, Category = "Datas|Sequence", meta = (ClampMin = 0.00, EditCondition = "bUseDeactivateCameraMove", EditConditionHides))
		float CollsionEventDelay;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		UProjectileMovementComponent* ProjectileMovementComponent;

	UPROPERTY(EditDefaultsOnly, Category = "Datas|Init")
		FName OverlapComponentTag = FName("OverlapCollision");


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
	virtual void Init(const FSpawnDamageDealerContext* InData)override;

	UFUNCTION(BlueprintCallable, BlueprintPure)AActor* GetTarget();
};
