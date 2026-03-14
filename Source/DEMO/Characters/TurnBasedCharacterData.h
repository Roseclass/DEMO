#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "Datas/UITypes.h"
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
class UBehaviorTree;
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

	// 슬롯 데이터
	UPROPERTY(Transient)
		FGameplayTag EquippedSkillTags[int32(ESkillSlotLocation::MAX)];

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Camera")
		FTransform SelectTargetTransform = FTransform(FVector3d(0, 0, 120));

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Camera")
		FTransform SelectSkillTransform;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Camera")
		FTransform SelectSkillRelativeTransform;
};

UCLASS(BlueprintType)
class DEMO_API UTurnBasedCharacterData : public UPrimaryDataAsset
{
	GENERATED_BODY()
public:
	// 캐릭터에 Init 때 복사할 데이터
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Runtime|Datas")
		FTurnBasedCharacterRuntimeData RuntimeData;

	// 캐릭터의 스킬 루트 태그
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Init|GAS")
		FGameplayTag SkillRootTag;

	// 시작 시 부여할 어빌리티 목록
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Init|GAS")
		TMap<FGameplayTag, FAbilitySpecInfo> GrantedAbilities;
	
	// 사용 가능한 어빌리티 목록
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Init|GAS")
		TMap<FGameplayTag, FAbilitySpecInfo> AvailableAbilities;

	// 시작 시 부여할 태그(예: State.Combat.Ready 등)
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Init|GAS")
		FGameplayTagContainer DefaultOwnedTags;

	// 시작 시 적용할 GE(버프/기본 스탯)
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Init|GAS")
		FAttributeInitialInfos AttributeInitialInfos;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Init|Mesh")
		TSoftObjectPtr<USkeletalMesh> SkeletalMesh;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Init|Mesh")
		TSoftClassPtr<UAnimInstance> AnimBlueprint;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Init|AI")
		TSoftObjectPtr<UBehaviorTree> BehaviorTree;

	UPROPERTY(EditAnywhere, Category = "Init|AI")
		FGameplayTag AIEquippedSkillTags[(int32)ESkillSlotLocation::MAX];
public:
	UTurnBasedCharacterData()
	{
		{	//Health
			FAttributeInitialStat MaxHealth;
			MaxHealth.Attribute = UAttributeSet_Character::GetMaxHealthAttribute();
			MaxHealth.Stat = 100;
			AttributeInitialInfos.InitalStats.Add(MaxHealth);
		}

		{	//Defense
			FAttributeInitialStat Defense;
			Defense.Attribute = UAttributeSet_Character::GetDefenseAttribute();
			Defense.Stat = 0;
			AttributeInitialInfos.InitalStats.Add(Defense);
		}

		{	//Power
			FAttributeInitialStat Power;
			Power.Attribute = UAttributeSet_Character::GetPowerAttribute();
			Power.Stat = 0;
			AttributeInitialInfos.InitalStats.Add(Power);
		}

		{	//Speed
			FAttributeInitialStat Speed;
			Speed.Attribute = UAttributeSet_Character::GetSpeedAttribute();
			Speed.Stat = 50;
			AttributeInitialInfos.InitalStats.Add(Speed);
		}
	};
protected:
	virtual void PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent) override
	{
		Super::PostEditChangeProperty(PropertyChangedEvent);

		if (!GrantedAbilities.Contains(FGameplayTag::RequestGameplayTag("Skill.System.Dead")))
			GrantedAbilities.FindOrAdd(FGameplayTag::RequestGameplayTag("Skill.System.Dead"));

		if (!GrantedAbilities.Contains(FGameplayTag::RequestGameplayTag("Skill.System.Hit")))
			GrantedAbilities.FindOrAdd(FGameplayTag::RequestGameplayTag("Skill.System.Hit"));

		UGameplayTagsManager & Manager = UGameplayTagsManager::Get();
		TSharedPtr<FGameplayTagNode> tagNode = Manager.FindTagNode(SkillRootTag);

		if (!tagNode.IsValid())return;
		for (auto i : tagNode->GetChildTagNodes())
		{
			if (AvailableAbilities.Contains(i->GetCompleteTag()))continue;
				AvailableAbilities.FindOrAdd(i->GetCompleteTag());
		}
	};
};
