#include "GameAbilities/AbilityComponent.h"
#include "Global.h"

#include "GameAbilities/AttributeSet_Character.h"
#include "GameAbilities/GA_BaseAbility.h"

UAbilityComponent::UAbilityComponent()
{

}

void UAbilityComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UAbilityComponent::TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	FGameplayTagContainer AllTags;
	GetOwnedGameplayTags(AllTags);
	for (auto tag : AllTags)
	{
		CLog::Print(tag.GetTagName().ToString(), -1, 0);
	}
}

void UAbilityComponent::Init(const TArray<FAbilitySpecInfo>& NewGAs)
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
	//for (auto i : Ability->GetActivatableAbilities())
	//{
	//	TArray<FGameplayTag> arr;
	//	i.Ability->AbilityTags.GetGameplayTagArray(arr);
	//	for (auto tag : arr)
	//		CLog::Print(tag.GetTagName().ToString());
	//}