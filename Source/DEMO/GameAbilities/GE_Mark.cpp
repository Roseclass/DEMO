#include "GameAbilities/GE_Mark.h"
#include "Global.h"
#include "GameAbilities/AttributeSet_Character.h"

UGE_Mark::UGE_Mark()
{
    DurationPolicy = EGameplayEffectDurationType::Infinite;
    StackingType = EGameplayEffectStackingType::AggregateByTarget;//턴스택을위해
    InheritableGameplayEffectTags.Added.AddTag(FGameplayTag::RequestGameplayTag("Effect.Damage.DoT"));// remove쿼리 식별용    
}

