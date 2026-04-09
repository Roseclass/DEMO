#pragma once

#include "Engine/DataAsset.h"
#include "GameplayTagContainer.h"
#include "BlackboardTypes.generated.h"

//#include "Characters/AI/BlackboardTypes.h"

class ATurnBasedCharacter;

UCLASS(BlueprintType)
class UTurnBasedBlackboardContainer : public UObject
{
	GENERATED_BODY()
public:
	UPROPERTY()FGameplayTag TargetSkillTag; // 선택할 스킬태그 << 서비스 계산결과 && 태스크 선택
	UPROPERTY()TArray<ATurnBasedCharacter*> Allies; // 아군 캐릭터들 << 서비스 계산용
	UPROPERTY()TArray<ATurnBasedCharacter*> Enemies; // 적 캐릭터들 << 서비스 계산용
	UPROPERTY()TArray<ATurnBasedCharacter*> SkillTargets; // 목표 캐릭터들 << 서비스 계산결과
	UPROPERTY()TArray<ATurnBasedCharacter*> SelectedTargets; // 선택된 캐릭터들 << 태스크 선택
};