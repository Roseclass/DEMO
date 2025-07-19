#include "GameAbilities/AttributeSet_Character.h"
#include "Global.h"
#include "GameplayEffectExtension.h"
#include "GameplayEffectTypes.h"

UAttributeSet_Character::UAttributeSet_Character()
{
    InitHealth(100);
    InitMaxHealth(100);
    InitMana(100);
    InitMaxMana(100);
    InitDefense(0);
    InitAdditiveDefense(0);
    InitMultiplicitiveDefense(100);
    InitPower(0);
    InitAdditivePower(0);
    InitMultiplicitivePower(100);
    InitSpeed(0);
    InitAdditiveSpeed(0);
    InitMultiplicitiveSpeed(100);
}

void UAttributeSet_Character::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
    Super::PreAttributeChange(Attribute, NewValue);

    //// If a Max value changes, adjust current to keep Current % of Current to Max
    //if (Attribute == GetTimeDilationAttribute())
    //{
    //    AActor* OwningActor = Cast<AActor>(GetOwningAbilitySystemComponent()->GetOwner());
    //    if (OwningActor)
    //        OwningActor->CustomTimeDilation = NewValue;
    //}
    //else if (Attribute == GetMaxHealthAttribute())
    //{
    //    AdjustAttributeForMaxChange(Health, MaxHealth, NewValue, GetHealthAttribute());
    //}
    //else if (Attribute == GetMaxManaAttribute())
    //{
    //    AdjustAttributeForMaxChange(Mana, MaxMana, NewValue, GetManaAttribute());
    //}
}

void UAttributeSet_Character::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
    /*
    * NOTE: PostGameplayEffectExecute()가 호출될 때 Attribute 변경은 이미 일어났지만,
    * 아직 클라이언트에 리플리케이트되지 않았으므로 여기에 값을 클램핑해도 클라이언트에 두 번의 네트워크 업데이트가 일어나지 않습니다.
    * 클라이언트는 클램핑 후에만 업데이트를 수신합니다.
    */

    Super::PostGameplayEffectExecute(Data);

    if (Data.EvaluatedData.Attribute == GetHealthAttribute())
    {
        if (GetHealth() <= 0)
        {
            UAbilitySystemComponent* AbilityComp = GetOwningAbilitySystemComponent();
            FGameplayTagContainer tags;
            tags.AddTag(FGameplayTag::RequestGameplayTag("Ability.Dead"));
            AbilityComp->TryActivateAbilitiesByTag(tags);
        }
        SetHealth(FMath::Clamp(GetHealth(), 0.0f, GetMaxHealth()));
    }
    else if (Data.EvaluatedData.Attribute == GetManaAttribute())
    {
        SetMana(FMath::Clamp(GetMana(), 0.0f, GetMaxMana()));
    }
}