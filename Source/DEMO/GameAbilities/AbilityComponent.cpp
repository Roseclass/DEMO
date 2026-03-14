#include "GameAbilities/AbilityComponent.h"
#include "Global.h"

#include "Characters/TurnBasedCharacterData.h"

#include "GameAbilities/AttributeSet_Character.h"
#include "GameAbilities/GA_BaseAbility.h"

UAbilityComponent::UAbilityComponent()
{

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
		if (activeEffect->Spec.DynamicAssetTags.HasTag(FGameplayTag::RequestGameplayTag("Effect.Damage.DoT")))
			DoTDamageHandles.Add(activeEffectHandle);
	}
	return activeEffectHandle;
}

void UAbilityComponent::InitGA(const TArray<FAbilitySpecInfo>& NewGAs)
{
	for (auto i : NewGAs)
	{
		if (!i.AbilityClass)continue;
		GiveAbility(FGameplayAbilitySpec(
			i.AbilityClass,
			i.Level,
			i.InputID,
			i.SourceObject
		));
	}

	//OnGameplayEffectAppliedDelegateToSelf.AddUFunction(this, "HitReaction");
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

void UAbilityComponent::PlayDeadSequence()
{
	FGameplayTagContainer tags;
	tags.AddTag(FGameplayTag::RequestGameplayTag("Skill.System.Dead"));
	TryActivateAbilitiesByTag(tags);
}

void UAbilityComponent::BroadcastOnSkillEnd()
{
	OnSkillEnd.Broadcast();
}

void UAbilityComponent::BroadcastOnDeadSequenceEnd()
{
	FTimerHandle WaitHandle;
	GetWorld()->GetTimerManager().SetTimer(WaitHandle, FTimerDelegate::CreateLambda([&]()
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