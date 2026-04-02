#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameplayTagContainer.h"
#include "GameAbilities/GameplayEffectContexts.h"
#include "TurnBasedEventResolver.generated.h"

class ATurnBasedCharacter;

UENUM(BlueprintType)
enum class EEffectEventResultType : uint8
{
    ReserveTurn,
    ReserveSkill,
    MAX UMETA(meta = (Hidden))
};

UENUM(BlueprintType)
enum class EEffectEventPhase : uint8
{
    Pre,
    Hit,
    MAX UMETA(meta = (Hidden))
};

UENUM(BlueprintType)
enum class EEffectEventTriggerCondition : uint8
{
    Always,
    TriggeredBySource,
    TriggeredByOther,
    MAX UMETA(meta = (Hidden))
};

USTRUCT(BlueprintType)
struct FEffectEventRule : public FTableRowBase
{
    GENERATED_BODY()
public:
    UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
        FGameplayTag TargetGrantedTag;

    UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
        FGameplayTag InstigatorGrantedTag;

    UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
        TObjectPtr<UDA_GCNPayload> ResultEventPayloads;

    UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
        EEffectEventPhase EventPhase;

    UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
        EEffectEventTriggerCondition TriggerCondition;

    UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
        EEffectEventResultType Result;
};

UCLASS()
class DEMO_API ATurnBasedEventResolver : public AActor
{
	GENERATED_BODY()
	
public:	
	ATurnBasedEventResolver();
protected:
	virtual void BeginPlay() override;
public:	
	virtual void Tick(float DeltaTime) override;

	//property
private:
	UDataTable* DT;
	TMap<FGameplayTag, TArray<FEffectEventRule*>> HitInstigatorRuleMap;
	TMap<FGameplayTag, TArray<FEffectEventRule*>> HitTargetRuleMap;
	TMap<FGameplayTag, TArray<FEffectEventRule*>> PreInstigatorRuleMap;
	TMap<FGameplayTag, TArray<FEffectEventRule*>> PreTargetRuleMap;
protected:
public:

	//function
private:
	void GetGrantedTagResourceArray(ATurnBasedCharacter* InCharacter, FGameplayTag TargetTag, TArray<ATurnBasedCharacter*>& ResultArray);
	bool CheckCondition(FEffectEventRule* Rule, ATurnBasedCharacter* TagSource, const FEffectEventContext* InEffectContext);
	void Execute(FEffectEventRule* Rule, ATurnBasedCharacter* TagSource, const FEffectEventContext* InEffectContext);
	void SolveEvent(const FEffectEventContext* InEffectContext, ATurnBasedCharacter* InCharacter, const TMap<FGameplayTag, TArray<FEffectEventRule*>>& RuleMap);
protected:
public:
	void SolveEvent(const FEffectEventContext* InEffectContext, EEffectEventPhase EffectEventPhase);
};
/*SolveInstigatorHitEvent SolveTargetHitEvent SolveInstigatorPreSkillEvent SolveTargetPreSkillEvent*/