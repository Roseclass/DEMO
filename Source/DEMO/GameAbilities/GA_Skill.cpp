#include "GameAbilities/GA_Skill.h"
#include "Global.h"
#include "GameplayCueManager.h"
#include "GameplayTagContainer.h"
#include "AbilitySystemGlobals.h"

#include "Characters/TurnBasedCharacter.h"
#include "GameAbilities/AbilityComponent.h"
#include "GameAbilities/AttributeSet_Character.h"
#include "GameAbilities/AbilityTaskTypes.h"
#include "GameAbilities/AT_MontageNotifyEvent.h"
#include "Objects/DamageDealer.h"

UGA_Skill::UGA_Skill()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	NextDamageDealerTriggerTag = FGameplayTag::RequestGameplayTag("Skill.System.NextDamageDealer");
}

void UGA_Skill::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	CameraMoveDataIdx = 0;
	DamageDealerDataIdx = 0;
	ApplyCameraMove();
}

float UGA_Skill::GetCooldownTimeRemaining(const FGameplayAbilityActorInfo* ActorInfo) const
{
	const UAbilitySystemComponent* const ASC = ActorInfo->AbilitySystemComponent.Get();
	if (ASC)
	{
		const FGameplayTagContainer* Tags = GetCooldownTags();
		if (Tags && Tags->Num() > 0)
		{
			FGameplayEffectQuery const Query = FGameplayEffectQuery::MakeQuery_MatchAnyOwningTags(*Tags);
			TArray< float > Durations = ASC->GetActiveEffectsTimeRemaining(Query);
			if (Durations.Num() > 0)
			{
				return CooldownLeft;
			}
		}
	}

	return 0.f;
}

bool UGA_Skill::CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags, const FGameplayTagContainer* TargetTags, OUT FGameplayTagContainer* OptionalRelevantTags) const
{
	return Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags);
}

void UGA_Skill::GetCooldownTimeRemainingAndDuration(FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, float& TimeRemaining, float& CooldownDuration) const
{
	TimeRemaining = 0.f;
	CooldownDuration = 0.f;

	const FGameplayTagContainer* Tags = GetCooldownTags();
	if (Tags && Tags->Num() > 0)
	{
		UAbilitySystemComponent* const AbilitySystemComponent = ActorInfo->AbilitySystemComponent.Get();
		check(AbilitySystemComponent != nullptr);

		FGameplayEffectQuery const Query = FGameplayEffectQuery::MakeQuery_MatchAnyOwningTags(*Tags);
		TArray< TPair<float, float> > DurationAndTimeRemaining = AbilitySystemComponent->GetActiveEffectsTimeRemainingAndDuration(Query);
		if (DurationAndTimeRemaining.Num() > 0)
		{
			TimeRemaining = CooldownLeft;
			CooldownDuration = CooldownTurns;
		}
	}
}

void UGA_Skill::ApplyCooldown(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo) const
{
	UGameplayEffect* CooldownGE = GetCooldownGameplayEffect();
	if (CooldownGE)
	{
		int32 lv = GetAbilityLevel();
		FGameplayEffectSpecHandle SpecHandle = MakeOutgoingGameplayEffectSpec(CooldownGE->GetClass(), 1);

		// 쿨다운 GameplayEffect의 지속 시간을 설정
		SpecHandle.Data.Get()->SetSetByCallerMagnitude(FGameplayTag::RequestGameplayTag("Skill.Cooldown"), GetCooldown(ActorInfo));

		// DynamicGrantedTags를 설정
		TArray<FGameplayTag> tags;
		GetCooldownTags()->GetGameplayTagArray(tags);
		for (auto tag : tags)
			SpecHandle.Data.Get()->DynamicGrantedTags.AddTag(tag);

		// 적용
		ApplyGameplayEffectSpecToOwner(Handle, ActorInfo, ActivationInfo, SpecHandle);
	}
}

const FGameplayTagContainer* UGA_Skill::GetCooldownTags() const
{
	// 쿨다운 동안 적용된 태그들을 가져온다
	FGameplayTagContainer* MutableTags = const_cast<FGameplayTagContainer*>(&TempCooldownTags);
	MutableTags->Reset(); // MutableTags는 CDO의 TempCooldownTags에 값을 쓰기 때문에, 어빌리티 쿨다운 태그가 변경되었을 경우를 대비해 이를 초기화해줘야 한다 (예: 다른 슬롯으로 이동된 경우)
	const FGameplayTagContainer* ParentTags = Super::GetCooldownTags();
	if (ParentTags)
	{
		MutableTags->AppendTags(*ParentTags);
	}
	MutableTags->AppendTags(CooldownTags);
	return MutableTags;
}

bool UGA_Skill::CheckCost(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, OUT FGameplayTagContainer* OptionalRelevantTags) const
{
	UGameplayEffect* CostGE = GetCostGameplayEffect();
	if (CostGE)
	{
		UAbilityComponent* ASC = Cast<UAbilityComponent>(ActorInfo->AbilitySystemComponent.Get());
		check(ASC != nullptr);

		float RequiredMana = GetCost(ActorInfo);
		float CurrentMana = ASC->GetNumericAttribute(UAttributeSet_Character::GetManaAttribute());

		if (RequiredMana > CurrentMana)
		{
			const FGameplayTag& CostTag = UAbilitySystemGlobals::Get().ActivateFailCostTag;

			if (OptionalRelevantTags && CostTag.IsValid())
			{
				OptionalRelevantTags->AddTag(CostTag);
			}
			return false;
		}
	}
	return true;
}

void UGA_Skill::ApplyCost(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo) const
{
	UGameplayEffect* CostGE = GetCostGameplayEffect();
	if (CostGE)
	{
		int32 lv = GetAbilityLevel();
		FGameplayEffectSpecHandle SpecHandle = MakeOutgoingGameplayEffectSpec(CostGE->GetClass(), lv);
		SpecHandle.Data->SetSetByCallerMagnitude(FGameplayTag::RequestGameplayTag("Skill.Cost"), GetCost(ActorInfo));
		ApplyGameplayEffectSpecToOwner(Handle, ActorInfo, ActivationInfo, SpecHandle);
	}
}

void UGA_Skill::SpawnDamageDealer()
{
	UAbilityComponent* asc = Cast<UAbilityComponent>(GetCurrentActorInfo()->AbilitySystemComponent);
	FGameplayCueParameters gameplayCueParameters;

	FSpawnDamageDealerContext* context = DamageDealerDatas[DamageDealerDataIdx++].Duplicate();
	context->AddInstigator(GetCurrentActorInfo()->OwnerActor.Get(), GetCurrentActorInfo()->AvatarActor.Get());
	context->TargetActor = asc->GetTarget();

	gameplayCueParameters.EffectContext = FGameplayEffectContextHandle(context);
	asc->ExecuteGameplayCue(FGameplayTag::RequestGameplayTag("GameplayCue.SpawnDamageDealer"), gameplayCueParameters);
}

void UGA_Skill::ApplyCameraMove()
{
	UAbilityComponent* asc = Cast<UAbilityComponent>(GetCurrentActorInfo()->AbilitySystemComponent);
	FGameplayCueParameters gameplayCueParameters;

	gameplayCueParameters.EffectContext = FGameplayEffectContextHandle(CameraMoveDatas[CameraMoveDataIdx++].Duplicate());
	gameplayCueParameters.EffectContext.AddInstigator(GetCurrentActorInfo()->OwnerActor.Get(), GetCurrentActorInfo()->AvatarActor.Get());

	asc->ExecuteGameplayCue(FGameplayTag::RequestGameplayTag("GameplayCue.TurnBasedCamera"), gameplayCueParameters);
}

void UGA_Skill::EventReceived(FGameplayTag EventTag, FGameplayEventData EventData)
{
	// 몽타주는 어빌리티가 끝나도 계속 재생된다.
	if (EventTag == EndTag)
	{
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
		return;
	}
	else if (EventTag == NextMontageTriggerTag)
	{
		PlayKeyMontage();
		PlaySubMontages();
		++MontageDataIdx;
		return;
	}
	else if(EventTag == NextCameraMoveTriggerTag)
	{
		ApplyCameraMove();
		return;
	}
	else if(EventTag == NextDamageDealerTriggerTag)
	{		
		SpawnDamageDealer();
		return;
	}
}

float UGA_Skill::GetCooldown(const FGameplayAbilityActorInfo* ActorInfo) const
{
	UAbilityComponent* ASC = Cast<UAbilityComponent>(ActorInfo->AbilitySystemComponent);
	return CooldownTurns;
}

float UGA_Skill::GetCost(const FGameplayAbilityActorInfo* ActorInfo) const
{
	UAbilityComponent* skill = Cast<UAbilityComponent>(ActorInfo->AbilitySystemComponent);
	int32 lv = GetAbilityLevel();
	return CostBase.GetValueAtLevel(lv);
}