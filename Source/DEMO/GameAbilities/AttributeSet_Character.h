#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "AbilitySystemComponent.h"
#include "AttributeSet_Character.generated.h"

/**
 * 
 */

#define ATTRIBUTE_ACCESSORS(ClassName, PropertyName) \
GAMEPLAYATTRIBUTE_PROPERTY_GETTER(ClassName, PropertyName) \
GAMEPLAYATTRIBUTE_VALUE_GETTER(PropertyName) \
GAMEPLAYATTRIBUTE_VALUE_SETTER(PropertyName) \
GAMEPLAYATTRIBUTE_VALUE_INITTER(PropertyName)

UCLASS()
class DEMO_API UAttributeSet_Character : public UAttributeSet
{
	GENERATED_BODY()
public:
	UAttributeSet_Character();
protected:
public:
	virtual void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override;
	virtual void PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data) override;

	//property
private:
    TMap<FGameplayAttribute, float> PreValue;
protected:
    UPROPERTY(BlueprintReadOnly, Category = "Status|Health")
        FGameplayAttributeData Health;

    UPROPERTY(BlueprintReadOnly, Category = "Status|Health")
        FGameplayAttributeData MaxHealth;

    UPROPERTY(BlueprintReadOnly, Category = "Status|Mana")
        FGameplayAttributeData Mana;

    UPROPERTY(BlueprintReadOnly, Category = "Status|Mana")
        FGameplayAttributeData MaxMana;

    UPROPERTY(BlueprintReadOnly, Category = "Status|Defense")
        FGameplayAttributeData Defense;

    UPROPERTY(BlueprintReadOnly, Category = "Status|Defense")
        FGameplayAttributeData AdditiveDefense;

    UPROPERTY(BlueprintReadOnly, Category = "Status|Defense")
        FGameplayAttributeData MultiplicitiveDefense;

    UPROPERTY(BlueprintReadOnly, Category = "Status|Power")
        FGameplayAttributeData Power;

    UPROPERTY(BlueprintReadOnly, Category = "Status|Power")
        FGameplayAttributeData AdditivePower;

    UPROPERTY(BlueprintReadOnly, Category = "Status|Power")
        FGameplayAttributeData MultiplicitivePower;

    UPROPERTY(BlueprintReadOnly, Category = "Status|Speed")
        FGameplayAttributeData Speed;

    UPROPERTY(BlueprintReadOnly, Category = "Status|Speed")
        FGameplayAttributeData AdditiveSpeed;

    UPROPERTY(BlueprintReadOnly, Category = "Status|Speed")
        FGameplayAttributeData MultiplicitiveSpeed;

    UPROPERTY(BlueprintReadOnly, Category = "Status|TurnBased")
        FGameplayAttributeData TurnGauge;

public:

	//function
private:
protected:
public:
    ATTRIBUTE_ACCESSORS(UAttributeSet_Character, Health);
    ATTRIBUTE_ACCESSORS(UAttributeSet_Character, MaxHealth);
    ATTRIBUTE_ACCESSORS(UAttributeSet_Character, Mana);
    ATTRIBUTE_ACCESSORS(UAttributeSet_Character, MaxMana);
    ATTRIBUTE_ACCESSORS(UAttributeSet_Character, Defense);
    ATTRIBUTE_ACCESSORS(UAttributeSet_Character, AdditiveDefense);
    ATTRIBUTE_ACCESSORS(UAttributeSet_Character, MultiplicitiveDefense);
    ATTRIBUTE_ACCESSORS(UAttributeSet_Character, Power);
    ATTRIBUTE_ACCESSORS(UAttributeSet_Character, AdditivePower);
    ATTRIBUTE_ACCESSORS(UAttributeSet_Character, MultiplicitivePower);
    ATTRIBUTE_ACCESSORS(UAttributeSet_Character, Speed);
    ATTRIBUTE_ACCESSORS(UAttributeSet_Character, AdditiveSpeed);
    ATTRIBUTE_ACCESSORS(UAttributeSet_Character, MultiplicitiveSpeed);
    ATTRIBUTE_ACCESSORS(UAttributeSet_Character, TurnGauge);
};

/*
* 경험치?
* 체력,마나,스킬포인트,속도
*/