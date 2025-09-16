#include "GameAbilities/GE_InstantDamage.h"
#include "Global.h"
#include "GameAbilities/AttributeSet_Character.h"

UGE_InstantDamage::UGE_InstantDamage()
{
    DurationPolicy = EGameplayEffectDurationType::Instant;

    FGameplayModifierInfo ModifierInfo;
    FSetByCallerFloat setByCaller;

    ModifierInfo.Attribute = UAttributeSet_Character::GetHealthAttribute();
    ModifierInfo.ModifierOp = EGameplayModOp::Additive;
    //setByCaller.DataTag = FGameplayTag::RequestGameplayTag(FName("Effect.Damage"));
    ModifierInfo.ModifierMagnitude = FGameplayEffectModifierMagnitude(setByCaller);
    Modifiers.Add(ModifierInfo);

    FGameplayEffectCue Cue;
    Cue.GameplayCueTags.AddTag(FGameplayTag::RequestGameplayTag(FName("GameplayCue.DamageText")));
    GameplayCues.Add(Cue);
}
