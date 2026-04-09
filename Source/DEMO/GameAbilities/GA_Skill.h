#pragma once

#include "CoreMinimal.h"
#include "GameAbilities/GA_MontageWithEvent.h"
#include "Engine/DataAsset.h"
#include "GameAbilities/AbilityUIEnums.h"
#include "GA_Skill.generated.h"

/**
 *
 */

class UAnimMontage;
class ADamageDealer;

UCLASS()
class DEMO_API UGA_Skill : public UGA_MontageWithEvent
{
	GENERATED_BODY()
public:
	UGA_Skill();
protected:
public:
	virtual void PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent);

	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	virtual bool CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags = nullptr, const FGameplayTagContainer* TargetTags = nullptr, OUT FGameplayTagContainer* OptionalRelevantTags = nullptr) const override;
	virtual float GetCooldownTimeRemaining(const FGameplayAbilityActorInfo* ActorInfo) const override;
	virtual void GetCooldownTimeRemainingAndDuration(FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, float& TimeRemaining, float& CooldownDuration) const override;
	virtual void ApplyCooldown(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo) const override;
	virtual const FGameplayTagContainer* GetCooldownTags() const override;
	virtual bool CheckCost(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, OUT FGameplayTagContainer* OptionalRelevantTags = nullptr) const override;
	virtual void ApplyCost(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo) const override;

	//property
private:
protected:
	int32 ApplyGEDataIdx;
	int32 ReserveActionDataIdx;
	int32 ScriptedMoveDataIdx;
	int32 SpawnDamageDealerDataIdx;

	UPROPERTY(EditAnywhere, Category = "Data")
		TArray<FSpawnDamageDealerContext> DamageDealerDatas;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Ability")
		ESkillTargetType TargetType;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Ability")
		float GoalCount = 1;

	UPROPERTY(EditAnywhere, Category = "Condition|Cost")
		FScalableFloat CostBase;

	UPROPERTY(EditDefaultsOnly, Category = "Condition|Cooldown")
		int32 CooldownTurns;

	UPROPERTY(Transient)
		FGameplayTagContainer TempCooldownTags;
public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Tags, meta = (Categories = "AbilityTagCategory"))
		FGameplayTag SkillTag;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Tags, meta = (Categories = "AbilityTagCategory"))
		FGameplayTag FXTriggerTag;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Tags, meta = (Categories = "AbilityTagCategory"))
		FGameplayTag NextApplyGETriggerTag;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Tags, meta = (Categories = "AbilityTagCategory"))
		FGameplayTag NextReserveActionTriggerTag;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Tags, meta = (Categories = "AbilityTagCategory"))
		FGameplayTag NextScriptedMoveTriggerTag;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Tags, meta = (Categories = "AbilityTagCategory"))
		FGameplayTag NextSpawnDamageDealerTriggerTag;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Tags, meta = (Categories = "AbilityTagCategory"))
		FGameplayTagContainer CooldownTags;


	//function
private:
protected:
	virtual void InitAbility()override;

	virtual void EventReceived(FGameplayTag EventTag, FGameplayEventData EventData)override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)override;
	
	virtual void SpawnFX(FGameplayTag InTag);
	virtual void ApplyGE();
	virtual void ReserveAction();
	virtual void ScriptedMove();
	virtual void SpawnDamageDealer();

	virtual float GetCooldown(const FGameplayAbilityActorInfo* ActorInfo)const;
	virtual float GetCost(const FGameplayAbilityActorInfo* ActorInfo)const;

	UFUNCTION(BlueprintImplementableEvent)float GetCalculatedDamage(int32 InLevel, float InPower, int32 InDamageDealerDataIndex)const;
public:
	FORCEINLINE FGameplayTag GetSkillTag() const { return SkillTag; }
	FORCEINLINE ESkillTargetType GetTargetType() const { return TargetType; }
	FORCEINLINE float GetGoalCount() const { return GoalCount; }
	TArray<float> GetCalculatedDamages(int32 InLevel, float InPower)const;
};

/*
* 태그에 skill.@@@.Sequence1 같은 식으로 받고
* 마지막 부분을 인덱스로 스킬시퀀스를 진행하는 방식으로하자
* 맵<태그,클래스> 이걸로 스폰
* 
* 쿨다운 적용 방식
* 쿨다운 태그는 블록 태그처럼 작동
* 쿨다운 태그를 Duration 방식으로 적용해서 일정 시간 동안 사용을 막는다고 보면됨
* 쿨다운 감지 또한 내가 정해둔 GE 태그가 적용되어 있는지 확인해보고
* 그중 가장 길게 남은 시간을 쿨다운으로 처리함
* 
* 1-1.기존 applycooldown을 infinite로 적용하고 사용이 오랜시간동안 안되는지 확인
* 1-2.checkcooldown 자체는 쿨다운 태그가 적용되어 있는지 확인만할뿐 그외에 작동은 하지 않음
* 2.안되는게 확인되었다면 GetCooldownTimeRemaining, GetCooldownTimeRemainingAndDuration를 재정의해서 수동으로 쿨다운을 리턴
* 3.턴진행에 따라 태그에 맞는 턴을 수동으로 걷어가야함
* 
* 
* 턴진행 어빌리티가 필요, 캐릭터마다 턴스피드 관련된 어트리뷰트가필요
* gamestate->next_turn 어빌리티
* 다음 턴의 캐릭터를 찾아 start turn(어빌리티) - 행동 선택(ui) - 실행(어빌리티) - end turn(어빌리티) 식으로 진행
* 
* 현재 SelectSkill 부분에서 액티브가 가능하면 전부 UI슬롯에 올려둔다.
* 스킬 슬롯 배치를 할때 액티브 가능 여부를 수정해야함
*/


/*
* 
* terra 사이클 테스트
* gideon 사이클 테스트
* minion ai추가 사이클 테스트
* healthbar추가
* 
* 
* 
fx 적용룰은 ge가 적용된걸 볼수있게, 스턴이 적용된걸 볼수있게, 체력이 닳는걸 확인할수있게 라는 기준이야
ge가 적용된건 특정 fx를 붙여주면 되겠고
스턴은 모션도 있지만 머리위에 공용스턴 fx를 붙일거고
체력이 닳는건 자체 커브를 이용해 잠깐 몸이 빨개지게 할거야
덤으로 하늘에서 떨어지는 모션을 greystone이 갖고있는데 그타이밍에 맞게 먼지같은거나 카메라 쉐잌 넣으면 될듯

gideon,morigesh,terra fx rule 마무리
revenantmarkfx stack 연출 딜레이 필요함
morigesh ult
gideon fx추가

매니저 ui업데이트 페이즈 추가
여러 이펙트 계산 적용시에 추가할 딜레이 연출 생각하기

death핸들 촘촘하게,승리패배판정 다시테스트
tps<->tb 오류있는지 검사
tb필드에선 tps 인풋 안먹게
배경,prop추가
순서로 진행하고 이거 다 마무리하면 완성임
*/