#include "GameAbilities/MMC_Damage.h"
#include "Global.h"
#include "AbilitySystemComponent.h"
#include "GameplayEffect.h"
#include "GameplayEffectExtension.h"

#include "GameAbilities/AttributeSet_Character.h"
#include "GameAbilities/GameplayEffectContexts.h"

UMMC_Damage::UMMC_Damage()
{
    DefenseDef.AttributeToCapture = UAttributeSet_Character::GetDefenseAttribute();
    DefenseDef.AttributeSource = EGameplayEffectAttributeCaptureSource::Target;
    DefenseDef.bSnapshot = false;

    RelevantAttributesToCapture.Add(DefenseDef);
}

float UMMC_Damage::CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const
{
    // Gather the tags from the source and target as that can affect which buffs should be used
    const FGameplayTagContainer* SourceTags = Spec.CapturedSourceTags.GetAggregatedTags();
    const FGameplayTagContainer* TargetTags = Spec.CapturedTargetTags.GetAggregatedTags();

    FAggregatorEvaluateParameters EvaluationParameters;
    EvaluationParameters.SourceTags = SourceTags;
    EvaluationParameters.TargetTags = TargetTags;

    float defense = 0.f;
    GetCapturedAttributeMagnitude(DefenseDef, Spec, EvaluationParameters, defense);
    defense = FMath::Max<float>(defense, 0.0f);

    float result = 0;
    const FDamageEffectContext* effectContext = static_cast<const FDamageEffectContext*>(Spec.GetEffectContext().Get());
    if (!effectContext)return result;

    result = effectContext->BaseDamage;
    result /= (1 + defense);

    return result;
}

float UMMC_Damage::CalculateDamageTextValue(const FDamageEffectContext* Context, UAbilitySystemComponent* TargetComponent)const
{
    float result = 0;
    if (!Context)return result;

    if (!TargetComponent)return result;

    const UAttributeSet_Character* attribute = Cast<UAttributeSet_Character>(TargetComponent->GetAttributeSet(UAttributeSet_Character::StaticClass()));
    if (!attribute)return result;

    float defense = defense = FMath::Max<float>(attribute->GetDefense(), 0.0f);

    result = Context->BaseDamage;
    result /= (1 + defense);

    return result;
}
