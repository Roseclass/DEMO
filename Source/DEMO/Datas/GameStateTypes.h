#pragma once

#include "CoreMinimal.h"
#include "GameStateTypes.generated.h"

/**
 *
 */

//#include "Datas/GameStateTypes.h"

UENUM(BlueprintType)
enum class EGameStatePhase : uint8
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
	UPROPERTY()EGameStatePhase CurrentPhase = EGameStatePhase::MAX;

	// 다음(목표) 페이즈
	UPROPERTY()EGameStatePhase NextPhase = EGameStatePhase::MAX;

	// 토큰 발급 시각(UTC 권장)
	UPROPERTY()FDateTime Timestamp = FDateTime(0);

public:
	static FPhaseTransitionToken Make(EGameStatePhase InCurrentPhase, EGameStatePhase InNextPhase)
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
		return Id.IsValid() && CurrentPhase != EGameStatePhase::MAX && NextPhase != EGameStatePhase::MAX && Timestamp != FDateTime(0);
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