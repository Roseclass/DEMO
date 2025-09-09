#pragma once

#include "CoreMinimal.h"
#include "GameInstanceTypes.generated.h"

/**
 *
 */

//#include "Datas/GameInstanceTypes.h"

class ABaseCharacter;

UENUM(BlueprintType)
enum class EGameInstancePhase : uint8
{
	MainMenu,
	TPS,
	TurnBased,
	MAX
};

USTRUCT()
struct FPhaseTransitionToken
{
	GENERATED_BODY()

public:
	// 고유 전이 식별자
	UPROPERTY()FGuid Id;

	// 현재 페이즈
	UPROPERTY()EGameInstancePhase CurrentPhase = EGameInstancePhase::MAX;

	// 다음(목표) 페이즈
	UPROPERTY()EGameInstancePhase NextPhase = EGameInstancePhase::MAX;

	// 토큰 발급 시각(UTC 권장)
	UPROPERTY()FDateTime Timestamp = FDateTime(0);

public:
	static FPhaseTransitionToken Make(EGameInstancePhase InCurrentPhase, EGameInstancePhase InNextPhase)
	{
		FPhaseTransitionToken T;
		T.Id = FGuid::NewGuid();
		T.CurrentPhase = InCurrentPhase;
		T.NextPhase = InNextPhase;
		T.Timestamp = FDateTime::UtcNow(); // 또는 Now()
		return T;
	}

	bool IsValid() const
	{
		return Id.IsValid() && CurrentPhase != EGameInstancePhase::MAX && NextPhase != EGameInstancePhase::MAX && Timestamp != FDateTime(0);
	}

	bool operator==(const FPhaseTransitionToken& Other) const 
	{ 
		if (Id != Other.Id)return 0;
		if (CurrentPhase != Other.CurrentPhase)return 0;
		if (NextPhase != Other.NextPhase)return 0;
		return Timestamp == Other.Timestamp;
	}
	FORCEINLINE bool operator!=(const FPhaseTransitionToken& Other) const { return !(*this == Other); }
};

UCLASS()
class UPhaseTransitionContext : public UObject
{
	GENERATED_BODY()
public:
	UPROPERTY() const AActor* Causer;        // 무기, 투사체
	UPROPERTY() const ABaseCharacter* Instigator;    // 공격자
	UPROPERTY() const ABaseCharacter* Target;        // 피해자
};