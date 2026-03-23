#include "GameAbilities/AbilityComponent.h"
#include "Global.h"

#include "Characters/TurnBasedCharacterData.h"

#include "GameAbilities/AttributeSet_Character.h"
#include "GameAbilities/GA_Skill.h"

UAbilityComponent::UAbilityComponent()
{
	StateTag_Dead = FGameplayTag::RequestGameplayTag("Skill.System.Dead");
	StateTag_Hit = FGameplayTag::RequestGameplayTag("Skill.System.Hit");
	StateTag_StunStart = FGameplayTag::RequestGameplayTag("Skill.System.StunStart");
	StateTag_StunEnd = FGameplayTag::RequestGameplayTag("Skill.System.StunEnd");
}

void UAbilityComponent::BeginPlay()
{
	Super::BeginPlay();
}

FActiveGameplayEffectHandle UAbilityComponent::ApplyGameplayEffectSpecToSelf(const FGameplayEffectSpec& GameplayEffect, FPredictionKey PredictionKey)
{
	FActiveGameplayEffectHandle activeEffectHandle = Super::ApplyGameplayEffectSpecToSelf(GameplayEffect, PredictionKey);
	const FActiveGameplayEffect* activeEffect = GetActiveGameplayEffect(activeEffectHandle);
	if (activeEffect)
	{
		for(auto tag : CooldownTags)
			if (activeEffect->Spec.DynamicGrantedTags.HasTagExact(tag))
			{
				CooldownHandles.Add(activeEffectHandle);
				break;
			}

		FGameplayTagContainer assetTags;
		activeEffect->Spec.GetAllAssetTags(assetTags);

		if (assetTags.HasTag(FGameplayTag::RequestGameplayTag("Effect.Buff")))
			BuffHandles.Add(activeEffectHandle);
		if (assetTags.HasTag(FGameplayTag::RequestGameplayTag("Effect.Debuff")))
			DebuffHandles.Add(activeEffectHandle);
		if (assetTags.HasTag(FGameplayTag::RequestGameplayTag("Effect.CC")))
			CCHandles.Add(activeEffectHandle);
		if (assetTags.HasTag(FGameplayTag::RequestGameplayTag("Effect.Damage.DoT")))
			DoTDamageHandles.Add(activeEffectHandle);		
	}
	return activeEffectHandle;
}

void UAbilityComponent::InitGA(const TArray<FAbilitySpecInfo>& NewGAs)
{
	for (auto i : NewGAs)
	{
		if (!i.AbilityClass)continue;
		GAHandles.Add(
		GiveAbility(FGameplayAbilitySpec(
			i.AbilityClass,
			i.Level,
			i.InputID,
			i.SourceObject
		)));
	}

	for (auto handle : GAHandles)
	{
		FGameplayAbilitySpec* spec = FindAbilitySpecFromHandle(handle);
		if (!spec)continue;		

		if (const UGA_Skill* skill = Cast<UGA_Skill>(spec->Ability))
			SkillGAHandles.FindOrAdd(skill->GetSkillTag()) = handle;

		if (spec->Ability->AbilityTags.HasTagExact(StateTag_Dead))StateGAHandles.FindOrAdd(StateTag_Dead) = handle;
		if (spec->Ability->AbilityTags.HasTagExact(StateTag_Hit))StateGAHandles.FindOrAdd(StateTag_Hit) = handle;
		if (spec->Ability->AbilityTags.HasTagExact(StateTag_StunStart))StateGAHandles.FindOrAdd(StateTag_StunStart) = handle;
		if (spec->Ability->AbilityTags.HasTagExact(StateTag_StunEnd))StateGAHandles.FindOrAdd(StateTag_StunEnd) = handle;


		//쿨다운 태그 저장
		if (spec->Ability->GetCooldownTags() && !spec->Ability->GetCooldownTags()->IsEmpty())
		{
			for (auto tag : *spec->Ability->GetCooldownTags())
			{
				if (CooldownTags.HasTagExact(tag))continue;
				CooldownTags.AddTag(tag);
			}
		}
	}

	if (!StateGAHandles.Contains(StateTag_Dead))CLog::Print(FString::Printf(TEXT("%s %s"), *GetAvatarActor()->GetName(), TEXT(" DeadHandle is not contains")));
	if (!StateGAHandles.Contains(StateTag_Hit))CLog::Print(FString::Printf(TEXT("%s %s"), *GetAvatarActor()->GetName(), TEXT(" HitHandle is not contains")));
	if (!StateGAHandles.Contains(StateTag_StunStart))CLog::Print(FString::Printf(TEXT("%s %s"), *GetAvatarActor()->GetName(), TEXT(" StunStartHandle is not contains")));
	if (!StateGAHandles.Contains(StateTag_StunEnd))CLog::Print(FString::Printf(TEXT("%s %s"), *GetAvatarActor()->GetName(), TEXT(" StunEndHandle is not contains")));
}

void UAbilityComponent::InitAttributes(const FAttributeInitialInfos* NewStats)
{
	for (auto i : NewStats->InitalStats)
	{
		UGameplayEffect* GE = NewObject<UGameplayEffect>(this);
		GE->DurationPolicy = EGameplayEffectDurationType::Instant;
		FGameplayModifierInfo info;
		info.Attribute = i.Attribute;
		info.ModifierOp = EGameplayModOp::Override;
		info.ModifierMagnitude = FScalableFloat(i.Stat);
		GE->Modifiers.Add(info);
		FGameplayEffectContextHandle context = MakeEffectContext();
		FGameplayEffectSpec Spec(GE, context);
		ApplyGameplayEffectSpecToSelf(Spec);
	}
}

void UAbilityComponent::RegisterPendingDeadArray()
{
	OnRegisterPendingDeadArray.Broadcast(GetAvatarActor());
}

void UAbilityComponent::BeginDeadAbility()
{
	CheckTrue_Print(!StateGAHandles.Contains(StateTag_Dead)," DeadHandle is not contains");
	TryActivateAbility(StateGAHandles[StateTag_Dead]);
}

void UAbilityComponent::BeginHitAbility()
{
	CheckTrue_Print(!StateGAHandles.Contains(StateTag_Hit)," HitHandle is not contains");
	TryActivateAbility(StateGAHandles[StateTag_Hit]);
}

void UAbilityComponent::EndStunAbility()
{
	CheckTrue_Print(!StateGAHandles.Contains(StateTag_StunEnd), " StunEndHandle is not contains");
	CancelAbilityHandle(StateGAHandles[StateTag_StunStart]);
	TryActivateAbility(StateGAHandles[StateTag_StunEnd]);
}

void UAbilityComponent::BroadcastOnSkillEnd()
{
	OnSkillEnd.Broadcast();
}

void UAbilityComponent::BroadcastOnDeadSequenceEnd()
{
	FTimerHandle WaitHandle;
	GetWorld()->GetTimerManager().SetTimer(WaitHandle, FTimerDelegate::CreateLambda([this]()
		{
			OnDeadSequenceEnd.Broadcast();
		}), 0.5, false);
}

void UAbilityComponent::GetAllDoTDamageHandles(OUT TArray<FActiveGameplayEffectHandle>& OutHandles)
{
	TArray<FActiveGameplayEffectHandle> remove;
	for (auto handle : DoTDamageHandles)
	{
		if (!GetActiveGameplayEffect(handle))
			remove.Add(handle);
		else
			OutHandles.Add(handle);
	}

	for (auto handle : remove)
		DoTDamageHandles.Remove(handle);
}

void UAbilityComponent::HandleCooldown()
{
	TArray<FActiveGameplayEffectHandle> remove;
	for (auto handle : CooldownHandles)
	{
		if (!GetActiveGameplayEffect(handle))
			remove.Add(handle);
		else
			RemoveActiveGameplayEffect(handle, 1);
	}

	for (auto handle : remove)
		CooldownHandles.Remove(handle);
}

void UAbilityComponent::HandleBuff()
{
	TArray<FActiveGameplayEffectHandle> remove;
	for (auto handle : BuffHandles)
	{
		if (!GetActiveGameplayEffect(handle))
			remove.Add(handle);
		else
			RemoveActiveGameplayEffect(handle, 1);
	}

	for (auto handle : remove)
		BuffHandles.Remove(handle);
}

void UAbilityComponent::HandleDebuff()
{
	TArray<FActiveGameplayEffectHandle> remove;
	for (auto handle : DebuffHandles)
	{
		if (!GetActiveGameplayEffect(handle))
			remove.Add(handle);
		else
			RemoveActiveGameplayEffect(handle, 1);
	}

	for (auto handle : remove)
		DebuffHandles.Remove(handle);
}

void UAbilityComponent::HandleCC()
{
	TArray<FActiveGameplayEffectHandle> remove;
	bool bStuned = 0;
	for (auto handle : CCHandles)
	{
		const FActiveGameplayEffect* effect = GetActiveGameplayEffect(handle);

		if (!effect)
		{
			remove.Add(handle);
			continue;
		}

		FGameplayTagContainer tags;
		effect->Spec.GetAllAssetTags(tags);

		if (tags.HasTag(FGameplayTag::RequestGameplayTag("Effect.CC.Stun")))
			bStuned = 1;

		RemoveActiveGameplayEffect(handle, 1);

		if (!GetActiveGameplayEffect(handle))remove.Add(handle);
	}

	for (auto handle : remove)
		CCHandles.Remove(handle);

	if (CCHandles.IsEmpty() && bStuned)
		EndStunAbility();
}

void UAbilityComponent::GetSkillCooldownTimeRemainingAndDuration(FGameplayTag InSkillTag, OUT float& TimeRemaining, OUT float& CooldownDuration)
{
	CheckTrue_Print(!SkillGAHandles.Contains(InSkillTag), " SkillGAHandles not contains InSkillTag!!");

	FGameplayAbilitySpec* spec = FindAbilitySpecFromHandle(SkillGAHandles[InSkillTag]);
	spec->Ability->GetCooldownTimeRemainingAndDuration(SkillGAHandles[InSkillTag],AbilityActorInfo.Get(), TimeRemaining, CooldownDuration);
}

FGameplayAbilitySpec* UAbilityComponent::FindAbilitySpecFromSkillTag(FGameplayTag InSkillTag)
{
	return SkillGAHandles.Contains(InSkillTag) ? FindAbilitySpecFromHandle(SkillGAHandles[InSkillTag]) : nullptr;
}

void UAbilityComponent::HandleDoTDamage(FActiveGameplayEffectHandle InHandle, OUT AActor** EventCauserActor, OUT AActor** EventTargetActor)
{
	if (!DoTDamageHandles.Contains(InHandle))
		CLog::Print("UAbilityComponent::HandleDoTDamage DoTDamageHandles NOT Contains InHandle!!");

	const FActiveGameplayEffect* activeEffect = GetActiveGameplayEffect(InHandle);
	CheckTrue_Print(!activeEffect, "activeEffect is nullptr!!");

	FGameplayEffectSpec spec = activeEffect->Spec;
	ActiveGameplayEffects.ExecuteActiveEffectsFrom(spec);

	*EventCauserActor = spec.GetEffectContext().GetInstigator();
	*EventTargetActor = GetAvatarActor();

	RemoveActiveGameplayEffect(InHandle, 1);
}

bool UAbilityComponent::HasTurnBlockingCC()
{
	for (auto handle : CCHandles)
	{
		const FActiveGameplayEffect* effect = GetActiveGameplayEffect(handle);
		if (!effect)continue;

		FGameplayTagContainer tags;
		effect->Spec.GetAllAssetTags(tags);

		if (tags.HasTag(FGameplayTag::RequestGameplayTag("Effect.CC.Stun")))
			return 1;
	}
	return 0;
}

float UAbilityComponent::GetHealth() const
{
	const UAttributeSet_Character* attribute = Cast<UAttributeSet_Character>(GetAttributeSet(UAttributeSet_Character::StaticClass()));

	return attribute->GetHealth();
}

float UAbilityComponent::GetMaxHealth() const
{
	const UAttributeSet_Character* attribute = Cast<UAttributeSet_Character>(GetAttributeSet(UAttributeSet_Character::StaticClass()));

	return attribute->GetMaxHealth();
}

float UAbilityComponent::GetDefense() const
{
	const UAttributeSet_Character* attribute = Cast<UAttributeSet_Character>(GetAttributeSet(UAttributeSet_Character::StaticClass()));

	float defense = attribute->GetDefense();
	float additiveDefense = attribute->GetAdditiveDefense();
	float multiplicitiveDefense = attribute->GetMultiplicitiveDefense() * 0.01;
	return (defense + additiveDefense) * multiplicitiveDefense;
}

float UAbilityComponent::GetPower() const
{
	const UAttributeSet_Character* attribute = Cast<UAttributeSet_Character>(GetAttributeSet(UAttributeSet_Character::StaticClass()));

	float power = attribute->GetPower();
	float additivePower = attribute->GetAdditivePower();
	float multiplicitivePower = attribute->GetMultiplicitivePower() * 0.01;
	return (power + additivePower) * multiplicitivePower;
}

float UAbilityComponent::GetSpeed() const
{
	const UAttributeSet_Character* attribute = Cast<UAttributeSet_Character>(GetAttributeSet(UAttributeSet_Character::StaticClass()));

	float speed = attribute->GetSpeed();
	float additiveSpeed = attribute->GetAdditiveSpeed();
	float multiplicitiveSpeed = attribute->GetMultiplicitiveSpeed() * 0.01;
	return (speed + additiveSpeed) * multiplicitiveSpeed;
}

float UAbilityComponent::GetTurnGauge() const
{
	const UAttributeSet_Character* attribute = Cast<UAttributeSet_Character>(GetAttributeSet(UAttributeSet_Character::StaticClass()));
	return attribute->GetTurnGauge();
}