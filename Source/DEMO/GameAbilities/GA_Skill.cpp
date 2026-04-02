#include "GameAbilities/GA_Skill.h"
#include "Global.h"
#include "GameplayCueManager.h"
#include "GameplayTagContainer.h"
#include "AbilitySystemGlobals.h"
#include "Abilities/Tasks/AbilityTask.h"

#include "Characters/TurnBasedCharacter.h"
#include "GameAbilities/AbilityComponent.h"
#include "GameAbilities/AttributeSet_Character.h"
#include "GameAbilities/AbilityTaskTypes.h"
#include "GameAbilities/AT_SkillNotifyEvent.h"
#include "Objects/DamageDealer.h"

UGA_Skill::UGA_Skill()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	NextApplyGETriggerTag = FGameplayTag::RequestGameplayTag("Skill.System.NextApplyGE");
	NextReserveActionTriggerTag = FGameplayTag::RequestGameplayTag("Skill.System.NextReserveAction");
	NextSpawnDamageDealerTriggerTag = FGameplayTag::RequestGameplayTag("Skill.System.NextDamageDealer");
	CHelpers::GetClass<UGameplayEffect>(&CooldownGameplayEffectClass, "Blueprint'/Game/GAS/GE/GE_Cooldown.GE_Cooldown_C'");
}

void UGA_Skill::PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	if (GoalCount < 1)GoalCount = 1;

	switch (TargetType)
	{
	case ESkillTargetType::Enemy: break;
	case ESkillTargetType::Ally: break;
	case ESkillTargetType::AllyButSelf: break;
	case ESkillTargetType::All: break;
	case ESkillTargetType::AllButSelf: break;
	case ESkillTargetType::Self:GoalCount = 1; break;
	default:break;
	}
}

void UGA_Skill::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
}

float UGA_Skill::GetCooldownTimeRemaining(const FGameplayAbilityActorInfo* ActorInfo) const
{
	float result = 0.0f;
	const UAbilitySystemComponent* const asc = ActorInfo->AbilitySystemComponent.Get();

	if (asc)
	{
		const FGameplayTagContainer* Tags = GetCooldownTags();
		if (Tags && Tags->Num() > 0)
		{
			FGameplayEffectQuery const Query = FGameplayEffectQuery::MakeQuery_MatchAnyOwningTags(*Tags);
			TArray<FActiveGameplayEffectHandle> effects = asc->GetActiveEffects(Query);
			for (const auto& effecthandle : effects)
			{
				const FActiveGameplayEffect* effect = asc->GetActiveGameplayEffect(effecthandle);
				result = UKismetMathLibrary::Max(result, effect->Spec.StackCount);
			}
			return result;
		}
	}

	return result;
}

bool UGA_Skill::CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags, const FGameplayTagContainer* TargetTags, OUT FGameplayTagContainer* OptionalRelevantTags) const
{
	return Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags);
}

void UGA_Skill::GetCooldownTimeRemainingAndDuration(FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, float& TimeRemaining, float& CooldownDuration) const
{
	TimeRemaining = GetCooldownTimeRemaining(ActorInfo);
	CooldownDuration = CooldownTurns;
}

void UGA_Skill::ApplyCooldown(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo) const
{
	/*
	* 쿨다운 effect는 지속시간 무한
	* 쿨다운 턴계산은 stack count
	*/

	CheckTrue_Print(!CooldownGameplayEffectClass, "CooldownGameplayEffectClass is nullptr");
	{
		int32 lv = GetAbilityLevel();
		FGameplayEffectSpecHandle SpecHandle = MakeOutgoingGameplayEffectSpec(CooldownGameplayEffectClass);
		SpecHandle.Data.Get()->StackCount = CooldownTurns;
		SpecHandle.Data.Get();
		// DynamicGrantedTags를 설정
		TArray<FGameplayTag> tags;
		GetCooldownTags()->GetGameplayTagArray(tags);
		for (auto tag : tags)
			SpecHandle.Data.Get()->DynamicGrantedTags.AddTag(tag);
		ApplyGameplayEffectSpecToOwner(Handle, ActorInfo, ActivationInfo, SpecHandle);

		// TODO::만약 쿨타임이 중복으로 적용되면? 앞에서 거르긴하는데 혹시라도 +될수도있음
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

void UGA_Skill::InitAbility()
{
	MontageDataIdx = 0;
	MoveCameraDataIdx = 0;
	ApplyGEDataIdx = 0;
	ReserveActionDataIdx = 0;
	SpawnDamageDealerDataIdx = 0;

	FTimerHandle WaitHandle;
	GetWorld()->GetTimerManager().SetTimer(WaitHandle, FTimerDelegate::CreateLambda([&]()
		{
			FGameplayTagContainer tags;
			tags.AddTag(NextMontageTriggerTag);
			tags.AddTag(NextCameraMoveTriggerTag);
			tags.AddTag(EndTag);
			tags.AddTag(FXTriggerTag);
			tags.AddTag(NextApplyGETriggerTag);
			tags.AddTag(NextReserveActionTriggerTag);
			tags.AddTag(NextSpawnDamageDealerTriggerTag);
			UAT_SkillNotifyEvent* task = UAT_SkillNotifyEvent::CreateSkillNotifyEvent(this, NAME_None, tags);
			task->EventReceived.AddDynamic(this, &UGA_Skill::EventReceived);

			// ReadyForActivation()는 C++에서 AbilityTask를 활성화 시킨다. Blueprint는 K2Node_LatentGameplayTaskCall에서 자동으로 ReadyForActivation()를 호출한다.
			task->ReadyForActivation();

			PlayKeyMontage();
			PlaySubMontages();
			MontageDataIdx++;

		}), MontageDatas[MontageDataIdx].StartDelay, false);
	MoveCamera();
}

void UGA_Skill::EventReceived(FGameplayTag EventTag, FGameplayEventData EventData)
{
	UAbilityComponent* asc = Cast<UAbilityComponent>(GetCurrentActorInfo()->AbilitySystemComponent);

	if (EventTag == EndTag)
	{
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
		asc->BroadcastOnSkillEnd();
		return;
	}
	else if (EventTag == NextMontageTriggerTag)
	{
		PlayKeyMontage();
		PlaySubMontages();
		++MontageDataIdx;
		return;
	}
	else if (EventTag == NextCameraMoveTriggerTag)
	{
		MoveCamera();
		return;
	}
	else if (EventTag.MatchesTag(FXTriggerTag))
	{
		SpawnFX(EventTag);
		return;
	}
	else if (EventTag == NextApplyGETriggerTag)
	{
		ApplyGE();
		return;
	}
	else if (EventTag == NextReserveActionTriggerTag)
	{
		ReserveAction();
		return;
	}
	else if (EventTag == NextSpawnDamageDealerTriggerTag)
	{
		SpawnDamageDealer();
		return;
	}
}

void UGA_Skill::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UGA_Skill::SpawnFX(FGameplayTag InTag)
{
	UAbilityComponent* asc = Cast<UAbilityComponent>(GetCurrentActorInfo()->AbilitySystemComponent);
	FGameplayCueParameters gameplayCueParameters;

	for (auto ch : asc->GetTargets())
	{
		FTurnBasedEffectContext* context = new FTurnBasedEffectContext();
		context->EffectSourceActor = GetCurrentActorInfo()->AvatarActor.Get();
		context->EffectCauserActor = GetCurrentActorInfo()->AvatarActor.Get();
		context->EffectTargetActor = ch;

		gameplayCueParameters.EffectContext = FGameplayEffectContextHandle(context);
		gameplayCueParameters.OriginalTag = InTag;
		asc->ExecuteGameplayCue(InTag, gameplayCueParameters);
	}
}

void UGA_Skill::ApplyGE()
{
	UDA_GCNPayload* payload = EventPayloads.Get();
	CheckTrue(!payload);
	CheckTrue(!payload->ApplyGE.IsValidIndex(ApplyGEDataIdx));

	UAbilityComponent* asc = Cast<UAbilityComponent>(GetCurrentActorInfo()->AbilitySystemComponent);
	FGameplayCueParameters gameplayCueParameters;

	FApplyGEContext* context = new FApplyGEContext();
	context->RuleSourceActor = GetCurrentActorInfo()->AvatarActor.Get();
	context->EventCauserActor = GetCurrentActorInfo()->AvatarActor.Get();
	for (auto ch : asc->GetTargets())
		context->EventTargetActors.Add(ch);
	context->Data = payload->ApplyGE[ApplyGEDataIdx++];

	gameplayCueParameters.EffectContext = FGameplayEffectContextHandle(context);
	asc->ExecuteGameplayCue(FGameplayTag::RequestGameplayTag("GameplayCue.ApplyGE"), gameplayCueParameters);
}

void UGA_Skill::ReserveAction()
{
	UDA_GCNPayload* payload = EventPayloads.Get();
	CheckTrue(!payload);
	CheckTrue(!payload->ReserveAction.IsValidIndex(ReserveActionDataIdx));

	UAbilityComponent* asc = Cast<UAbilityComponent>(GetCurrentActorInfo()->AbilitySystemComponent);
	FGameplayCueParameters gameplayCueParameters;

	FReserveActionContext* context = new FReserveActionContext();
	context->RuleSourceActor = GetCurrentActorInfo()->AvatarActor.Get();
	context->EventCauserActor = GetCurrentActorInfo()->AvatarActor.Get();
	for (auto ch : asc->GetTargets())
		context->EventTargetActors.Add(ch);
	context->Data = payload->ReserveAction[ReserveActionDataIdx++];

	gameplayCueParameters.EffectContext = FGameplayEffectContextHandle(context);
	asc->ExecuteGameplayCue(FGameplayTag::RequestGameplayTag("GameplayCue.ReserveAction"), gameplayCueParameters);
}

void UGA_Skill::SpawnDamageDealer()
{
	UDA_GCNPayload* payload = EventPayloads.Get();
	CheckTrue(!payload);
	CheckTrue(!payload->SpawnDamageDealer.IsValidIndex(SpawnDamageDealerDataIdx));

	UAbilityComponent* asc = Cast<UAbilityComponent>(GetCurrentActorInfo()->AbilitySystemComponent);
	FGameplayCueParameters gameplayCueParameters;

	FSpawnDamageDealerContext* context = new FSpawnDamageDealerContext();
	context->RuleSourceActor = GetCurrentActorInfo()->AvatarActor.Get();
	context->EventCauserActor = GetCurrentActorInfo()->AvatarActor.Get();
	for (auto ch : asc->GetTargets())
		context->EventTargetActors.Add(ch);
	context->Data = payload->SpawnDamageDealer[SpawnDamageDealerDataIdx++];

	gameplayCueParameters.EffectContext = FGameplayEffectContextHandle(context);
	asc->ExecuteGameplayCue(FGameplayTag::RequestGameplayTag("GameplayCue.SpawnDamageDealer"), gameplayCueParameters);
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

TArray<float> UGA_Skill::GetCalculatedDamages(int32 InLevel, float InPower)const
{
	TArray<float> result;
	for (int32 i = 0; i < DamageDealerDatas.Num(); i++)
		result.Add(GetCalculatedDamage(InLevel, InPower, i));
	return result;
}
