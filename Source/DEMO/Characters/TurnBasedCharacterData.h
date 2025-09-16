#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "GameplayTagContainer.h"
#include "GameplayTagsManager.h"
#include "GameAbilities/AbilityTypes.h"
#include "GameAbilities/AttributeSet_Character.h"
#include "TurnBasedCharacterData.generated.h"

/**
 *
 */

//#include "Characters/TurnBasedCharacterData.h"

class UAnimInstance;
class USkeletalMesh;
class UGameplayEffect;
class UGA_BaseAbility;
class AEventTrigger;

USTRUCT(BlueprintType)
struct FTurnBasedCharacterRuntimeData
{
	GENERATED_BODY()
public:
	bool bInitComplete = 0;
	// 서브 시스템 (레지스트리->맵)에서의 키값
	UPROPERTY(Transient)
		FGameplayTag DataTag;
};

UCLASS(BlueprintType)
class DEMO_API UTurnBasedCharacterData : public UPrimaryDataAsset
{
	GENERATED_BODY()
public:
	UTurnBasedCharacterData()
	{
		{	//Defense
			FGameplayModifierInfo Mod;
			Mod.Attribute = UAttributeSet_Character::GetDefenseAttribute();
			Mod.ModifierOp = EGameplayModOp::Additive;
			Mod.ModifierMagnitude = FScalableFloat();
			AttributeInitialInfo.InitalStats.Add(Mod); 
		}

		{	//Power
			FGameplayModifierInfo Mod;
			Mod.Attribute = UAttributeSet_Character::GetPowerAttribute();
			Mod.ModifierOp = EGameplayModOp::Additive;
			Mod.ModifierMagnitude = FScalableFloat();
			AttributeInitialInfo.InitalStats.Add(Mod); 
		}

		{	//Speed
			FGameplayModifierInfo Mod;
			Mod.Attribute = UAttributeSet_Character::GetSpeedAttribute();
			Mod.ModifierOp = EGameplayModOp::Additive;
			Mod.ModifierMagnitude = FScalableFloat();
			AttributeInitialInfo.InitalStats.Add(Mod); 
		}

	}
protected:
	virtual void PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent) override
	{
		Super::PostEditChangeProperty(PropertyChangedEvent);

		GrantedAbilities.Empty();

		UGameplayTagsManager& Manager = UGameplayTagsManager::Get();
		TSharedPtr<FGameplayTagNode> tagNode = Manager.FindTagNode(SkillRootTag);

		if(!tagNode.IsValid())return;
		for (auto i : tagNode->GetChildTagNodes())
			GrantedAbilities.FindOrAdd(i->GetCompleteTag());
	};
public:
	// 캐릭터에 Init 때 복사할 데이터
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Runtime|Datas")
		FTurnBasedCharacterRuntimeData RuntimeData;

	// 시작 시 부여할 태그(예: State.Combat.Ready 등)
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Init|GAS")
		FGameplayTag SkillRootTag;

	// 시작 시 부여할 어빌리티 목록
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Init|GAS")
		TMap<FGameplayTag, FAbilitySpecInfo> GrantedAbilities;

	// 시작 시 부여할 태그(예: State.Combat.Ready 등)
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Init|GAS")
		FGameplayTagContainer DefaultOwnedTags;

	// 시작 시 적용할 GE(버프/기본 스탯)
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Init|GAS")
		FAttributeInitialInfo AttributeInitialInfo;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Init|Mesh")
		TSoftObjectPtr<USkeletalMesh> SkeletalMesh;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Init|Mesh")
		TSoftClassPtr<UAnimInstance> AnimBlueprint;
};
