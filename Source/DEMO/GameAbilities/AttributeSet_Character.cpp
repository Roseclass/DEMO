#include "GameAbilities/AttributeSet_Character.h"
#include "Global.h"
#include "GameplayEffectExtension.h"
#include "GameplayEffectTypes.h"
#include "GameplayCueManager.h"

#include "GameAbilities/AbilityComponent.h"
#include "GameAbilities/GameplayEffectContexts.h"

UAttributeSet_Character::UAttributeSet_Character()
{
    InitHealth(10);
    InitMaxHealth(10);
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

}

void UAttributeSet_Character::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
    /*
    * NOTE: PostGameplayEffectExecute()가 호출될 때 Attribute 변경은 이미 일어났지만,
    * 아직 클라이언트에 리플리케이트되지 않았으므로 여기에 값을 클램핑해도 클라이언트에 두 번의 네트워크 업데이트가 일어나지 않습니다.
    * 클라이언트는 클램핑 후에만 업데이트를 수신합니다.
    */

    Super::PostGameplayEffectExecute(Data);

    AActor* OwningActor = Cast<AActor>(GetOwningAbilitySystemComponent()->GetOwner());

    if (Data.EvaluatedData.Attribute == GetHealthAttribute())
    {
        // damagetext
        {
            FDamageEffectContext* context = new FDamageEffectContext();        
            context->CalculatedDamage = FMath::Abs(Data.EvaluatedData.Magnitude);
            context->Location = OwningActor->GetActorLocation();

            FGameplayEffectContextHandle EffectContextHandle = FGameplayEffectContextHandle(context);
            EffectContextHandle.AddInstigator(Data.EffectSpec.GetContext().GetInstigator(), Data.EffectSpec.GetContext().GetEffectCauser());

            GetOwningAbilitySystemComponent()->ExecuteGameplayCue(FGameplayTag::RequestGameplayTag("GameplayCue.DamageText"), EffectContextHandle);
        }

        {
            UAbilityComponent* asc = Cast<UAbilityComponent>(GetOwningAbilitySystemComponent());
            if (asc && GetHealth() <= 0.f)
            {
                //사망예약
                asc->RegisterPendingDeadArray();
            }
            else if (asc && Data.EvaluatedData.Magnitude < 0.f)
            {
                //맞는모션재생
                asc->BeginHitAbility();
            }
        }

        SetHealth(FMath::Clamp(GetHealth(), 0.0f, GetMaxHealth()));
    }
    else if (Data.EvaluatedData.Attribute == GetMaxHealthAttribute())
    {
        float newHealth = GetMaxHealth();
        if (Data.EvaluatedData.ModifierOp == EGameplayModOp::Additive)
            newHealth = (GetHealth() + GetMaxHealth() - Data.EvaluatedData.Magnitude);
        SetHealth(newHealth);        
    }
    else if (Data.EvaluatedData.Attribute == GetManaAttribute())
    {
        SetMana(FMath::Clamp(GetMana(), 0.0f, GetMaxMana()));
    }
}