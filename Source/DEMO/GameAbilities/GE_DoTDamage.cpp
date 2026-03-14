#include "GameAbilities/GE_DoTDamage.h"
#include "Global.h"
#include "GameAbilities/AttributeSet_Character.h"

UGE_DoTDamage::UGE_DoTDamage()
{
    DurationPolicy = EGameplayEffectDurationType::Infinite;
    StackingType = EGameplayEffectStackingType::AggregateByTarget;//턴스택을위해
    InheritableGameplayEffectTags.Added.AddTag(FGameplayTag::RequestGameplayTag("Effect.Damage.DoT"));// remove쿼리 식별용
}

