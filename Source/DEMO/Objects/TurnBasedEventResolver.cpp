#include "Objects/TurnBasedEventResolver.h"
#include "Global.h"
#include "GameAbilities/AbilityComponent.h"

#include "TurnBasedSubsystem.h"

#include "Characters/TurnBasedCharacter.h"

ATurnBasedEventResolver::ATurnBasedEventResolver()
{
	PrimaryActorTick.bCanEverTick = true;
	CHelpers::GetAsset<UDataTable>(&DT, "DataTable'/Game/Datas/DT_EffectEventRules.DT_EffectEventRules'");
}

void ATurnBasedEventResolver::BeginPlay()
{
	Super::BeginPlay();
	TArray<FEffectEventRule*> datas;
	DT->GetAllRows<FEffectEventRule>("", datas);
	for (auto data : datas)
	{
		if (data->EventPhase == EEffectEventPhase::Hit)
		{
			HitInstigatorRuleMap.FindOrAdd(data->InstigatorGrantedTag).Add(data);
			HitTargetRuleMap.FindOrAdd(data->TargetGrantedTag).Add(data);
		}
		if (data->EventPhase == EEffectEventPhase::Pre)
		{
			PreInstigatorRuleMap.FindOrAdd(data->InstigatorGrantedTag).Add(data);
			PreTargetRuleMap.FindOrAdd(data->TargetGrantedTag).Add(data);
		}
	}
}

void ATurnBasedEventResolver::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ATurnBasedEventResolver::GetGrantedTagResourceArray(ATurnBasedCharacter* InCharacter, FGameplayTag TargetTag, TArray<ATurnBasedCharacter*>& ResultArray)
{
	CheckNull(InCharacter);
	UAbilitySystemComponent* asc = InCharacter->GetAbilitySystemComponent();
	TArray<FGameplayEffectSpec>specs;
	asc->GetAllActiveGameplayEffectSpecs(specs);
	for (auto spec : specs)
	{
		for (auto tag : spec.DynamicGrantedTags)
		{
			if (TargetTag != tag)continue;
			ResultArray.Add(Cast<ATurnBasedCharacter>(spec.GetContext().GetInstigator()));
		}
	}
}

bool ATurnBasedEventResolver::CheckCondition(FEffectEventRule* Rule, ATurnBasedCharacter* TagSource, const FEffectEventContext* InEffectContext)
{
	if (!TagSource || !InEffectContext->EventCauserActor.Get())return 0;
	if (Rule->TriggerCondition == EEffectEventTriggerCondition::Always)return 1;
	else if (Rule->TriggerCondition == EEffectEventTriggerCondition::TriggeredBySource
		&& InEffectContext->EventCauserActor == TagSource)
		return 1;
	else if (Rule->TriggerCondition == EEffectEventTriggerCondition::TriggeredByOther
		&& InEffectContext->EventCauserActor != TagSource)
		return 1;
	return 0;
}

void ATurnBasedEventResolver::Execute(FEffectEventRule* Rule, ATurnBasedCharacter* TagSource, const FEffectEventContext* InEffectContext)
{
	//UNDONE::GCN대체필요

	if (Rule->TriggerContext.GCNTag == FGameplayTag::EmptyTag)return;

	UAbilityComponent* asc = Cast<UAbilityComponent>(TagSource->GetAbilitySystemComponent());
	FGameplayCueParameters gameplayCueParameters;

	FPayloadContext* payload = Rule->TriggerContext.Duplicate();
	payload->EventCauserActor = InEffectContext->EventCauserActor;
	payload->EventTargetActor = InEffectContext->EventTargetActor;
	payload->RuleSourceActor = TagSource;

	gameplayCueParameters.EffectContext = FGameplayEffectContextHandle(payload);
	asc->ExecuteGameplayCue(payload->GCNTag, gameplayCueParameters);
}

void ATurnBasedEventResolver::SolveEvent(const FEffectEventContext* InEffectContext, ATurnBasedCharacter* InCharacter, const TMap<FGameplayTag, TArray<FEffectEventRule*>>& RuleMap)
{
	CheckTrue_Print(!InCharacter, "instigator is nullptr");
	UAbilitySystemComponent* asc = InCharacter->GetAbilitySystemComponent();

	/*
	* TODO::n^3으로 돌아가는거 줄이기? 근데 이거 말고 방법이 있음?? 양 얼마나 된다고 그냥 굴려도 되는거 아님??
	* 
	* specs모두 불러오기 - grantedtag 검사 - 일치하는 rule 찾기 - 검사 - 실행
	*/

	TArray<FGameplayEffectSpec>specs;
	asc->GetAllActiveGameplayEffectSpecs(specs);
	for (const auto& spec : specs)
	{
		FGameplayTagContainer tags;
		spec.GetAllGrantedTags(tags);
		for (const auto& tag : tags)
		{
			const TArray<FEffectEventRule*>* rules = RuleMap.Find(tag);
			if (!rules)continue;

			for (auto rule : *rules)
			{
				ATurnBasedCharacter* source = Cast<ATurnBasedCharacter>(spec.GetContext().GetInstigator());
				if (!source)continue;
				if (!CheckCondition(rule, source, InEffectContext))continue;
				CLog::Print("__FUNCTION__");
				Execute(rule, source, InEffectContext);
			}
		}
	}
}

void ATurnBasedEventResolver::SolveEvent(const FEffectEventContext* InEffectContext, EEffectEventPhase EffectEventPhase)
{
	if (EffectEventPhase == EEffectEventPhase::Hit)
	{
		SolveEvent(InEffectContext, Cast<ATurnBasedCharacter>(InEffectContext->EventCauserActor), HitInstigatorRuleMap);
		SolveEvent(InEffectContext, Cast<ATurnBasedCharacter>(InEffectContext->EventTargetActor), HitTargetRuleMap);
	}
	else if (EffectEventPhase == EEffectEventPhase::Pre)
	{
		SolveEvent(InEffectContext, Cast<ATurnBasedCharacter>(InEffectContext->EventCauserActor), PreInstigatorRuleMap);
		SolveEvent(InEffectContext, Cast<ATurnBasedCharacter>(InEffectContext->EventTargetActor), PreTargetRuleMap);
	}
}
