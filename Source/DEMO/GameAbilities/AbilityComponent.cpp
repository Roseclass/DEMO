#include "GameAbilities/AbilityComponent.h"
#include "Global.h"

#include "GameAbilities/AttributeSet_Character.h"

UAbilityComponent::UAbilityComponent()
{

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
