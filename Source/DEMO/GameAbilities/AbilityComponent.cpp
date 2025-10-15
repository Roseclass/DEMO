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

void UAbilityComponent::InitAttributes(const FAttributeInitialInfo* NewStats)
{
	for (auto i : NewStats->InitalStats)
	{
		UGameplayEffect* GE = NewObject<UGameplayEffect>(this);
		GE->DurationPolicy = EGameplayEffectDurationType::Instant;
		GE->Modifiers.Add(i);
		FGameplayEffectContextHandle context = MakeEffectContext();
		FGameplayEffectSpec Spec(GE, context);
		ApplyGameplayEffectSpecToSelf(Spec);
	}
}

void UAbilityComponent::PlayDeadSequence()
{
	CLog::Print("PlayDeadSequence");
}

void UAbilityComponent::BroadcastOnSkillEnd()
{
	OnSkillEnd.Broadcast();
}

void UAbilityComponent::BroadcastOnDeadSequenceEnd()
{
	OnDeadSequenceEnd.Broadcast();
}

float UAbilityComponent::GetHealth() const
{
	const UAttributeSet_Character* attribute = Cast<UAttributeSet_Character>(GetAttributeSet(UAttributeSet_Character::StaticClass()));

	return attribute->GetHealth();
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