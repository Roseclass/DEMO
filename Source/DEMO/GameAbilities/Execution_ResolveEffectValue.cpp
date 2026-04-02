#include "GameAbilities/Execution_ResolveEffectValue.h"
#include "Global.h"

#include "Characters/TurnBasedCharacter.h"

#include "GameAbilities/AbilityComponent.h"
#include "GameAbilities/AttributeSet_Character.h"
#include "GameAbilities/GameplayEffectContexts.h"

UExecution_ResolveEffectValue::UExecution_ResolveEffectValue()
{
	UDataTable* ptr;
	CHelpers::GetAsset<UDataTable>(&ptr, "DataTable'/Game/Datas/DT_ExecutionRules.DT_ExecutionRules'");
	
	ResolveRules = ptr;
}

void UExecution_ResolveEffectValue::Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams, FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const
{
	Super::Execute_Implementation(ExecutionParams, OutExecutionOutput);

	const FTurnBasedEffectContext* context = static_cast<const FTurnBasedEffectContext*>(ExecutionParams.GetOwningSpec().GetEffectContext().Get());
	CheckTrue_Print(!context, "context cast FAILED!!");

	float tempAdditive = 0;
	float tempMultiplier = 1;

	for (const auto& data : ModifyDatas)
		HandleModifyData(context, data, tempAdditive, tempMultiplier);

	HandleResolveRules(context, tempAdditive, tempMultiplier);
	HandleCamerMoveContext(context);

	FGameplayModifierEvaluatedData data;
	data.Attribute = Attribute;
	
	{
		float value = (BaseValue + tempAdditive) * tempMultiplier;
		if (ResolveType == EExecutionResolveType::Heal && value < 0.f)value = 0.f;
		else if (ResolveType == EExecutionResolveType::Damage)
		{
			value *= -1;
			if(0.f <= value)value = 0.f;
		}
		data.Magnitude = value;
	}

	data.ModifierOp = EGameplayModOp::Additive;
	OutExecutionOutput.AddOutputModifier(data);

	ATurnBasedCharacter* ch = Cast<ATurnBasedCharacter>(context->EffectCauserActor.Get());
	FGameplayCueParameters params;
	params.OriginalTag = FXTag;
	params.EffectContext = FGameplayEffectContextHandle(context->Duplicate());
	ch->GetAbilitySystemComponent()->ExecuteGameplayCue(FXTag, params);
}

void UExecution_ResolveEffectValue::HandleResolveRules(const FTurnBasedEffectContext* InContext, OUT float& Additive, OUT float& Multiplier)const
{
	TArray<FExecutionModifyRule*> arr;
	ResolveRules->GetAllRows("", arr);
	for (auto rule : arr)
	{
		if (rule->ResolveType != ResolveType)continue;
		HandleModifyData(InContext, rule->ResolveData, Additive, Multiplier);
	}
}

void UExecution_ResolveEffectValue::HandleModifyData(const FTurnBasedEffectContext* InContext, const FExecutionModifyData& InData, OUT float& Additive, OUT float& Multiplier) const
{
	TArray<ATurnBasedCharacter*> refArr;

	// 레퍼런스 액터 설정
	switch (InData.ReferenceActor)
	{
	case EExecutionReferenceActorType::EffectSource:refArr.Add(Cast<ATurnBasedCharacter>(InContext->EffectSourceActor.Get())); break;
	case EExecutionReferenceActorType::EffectTarget:refArr.Add(Cast<ATurnBasedCharacter>(InContext->EffectTargetActor.Get())); break;
	case EExecutionReferenceActorType::EffectCauser:refArr.Add(Cast<ATurnBasedCharacter>(InContext->EffectCauserActor.Get())); break;
	case EExecutionReferenceActorType::SkillTargets:
	{
		for (auto ptr : InContext->SkillTargetActors)
			refArr.Add(Cast<ATurnBasedCharacter>(ptr.Get()));
		break; 	
	}
	case EExecutionReferenceActorType::SkillCauser:refArr.Add(Cast<ATurnBasedCharacter>(InContext->SkillCauserActor.Get())); break;
	default:break;
	}

	TArray<float> countArr;
	for (auto ch : refArr)
	{
		bool bApply = 0;
		float cnt = 0;
		EvaluateModifyData(ch, InData, bApply, cnt);
		if (!bApply)continue;
		countArr.Add(cnt);
	}
	countArr.Sort();

	while (countArr.Num() > InData.ReferenceActorCount && InData.ReferenceActor == EExecutionReferenceActorType::SkillTargets)
	{
		if (InData.SkillTargetSelectType == EExecutionSelectType::All)break;
		else if (InData.SkillTargetSelectType == EExecutionSelectType::Top)
			countArr.RemoveAt(0);
		else if (InData.SkillTargetSelectType == EExecutionSelectType::Bottom)
			countArr.RemoveAt(countArr.Num() - 1);
		else if (InData.SkillTargetSelectType == EExecutionSelectType::Random)
			countArr.RemoveAt(UKismetMathLibrary::RandomIntegerInRange(0, countArr.Num() - 1));
	}

	float cnt = 0;

	if (InData.SkillTargetAggregationType == EExecutionAggregationType::Sum)
		for (auto i : countArr)cnt += i;
	else if (InData.SkillTargetAggregationType == EExecutionAggregationType::Average)
	{
		for (auto i : countArr)
			cnt += i;
		if (countArr.Num())cnt /= countArr.Num();
		else cnt = 0;
	}

	switch (InData.ModifyOp)
	{
	case EExecutionModifyOp::AddConstant:Additive += InData.Value; break;
	case EExecutionModifyOp::AddMeasuredValueScaled:Additive += (cnt * InData.Value); break;
	case EExecutionModifyOp::MultiplyConstant:Multiplier *= InData.Value; break;
	case EExecutionModifyOp::MultiplyFromMeasuredValue:Multiplier *= (cnt * InData.Value); break;
	default:break;
	}
}

void UExecution_ResolveEffectValue::EvaluateModifyData(ATurnBasedCharacter* Ref, const FExecutionModifyData& InData, OUT bool& bApply, OUT float& Count)const
{
	CheckTrue_Print(!Ref, "target cast FAILED!!");
	UAbilityComponent* asc = Cast<UAbilityComponent>(Ref->GetAbilitySystemComponent());
	CheckTrue_Print(!asc, "asc cast FAILED!!");

	// always라면 무조건 적용
	if (InData.MeasureType == EExecutionMeasureType::Always)
		bApply = 1;
	else if (InData.MeasureType == EExecutionMeasureType::HasTag || InData.MeasureType == EExecutionMeasureType::HasTagExact)
	{
		FGameplayTagContainer tags;
		asc->GetOwnedGameplayTags(tags);

		// HasTagExact는 숫자 판정x 찾으면 바로 pass
		if (InData.MeasureType == EExecutionMeasureType::HasTag)
		{
			// 하위 태그 숫자 판정
			for (auto tag : tags)
				if (tag.MatchesTag(InData.Tag))Count += 1;
		}
		else if (InData.MeasureType == EExecutionMeasureType::HasTagExact)
		{
			if (tags.HasAnyExact(FGameplayTagContainer(InData.Tag)))
			{
				bApply = 1;
				Count = 1;
			}
		}
	}
	else if (InData.MeasureType == EExecutionMeasureType::Attribute)
	{
		// 어트리뷰트 값 불러오기
		if (InData.AttributeType == EExecutionAttributeType::Health)
			Count = asc->GetHealth();
		else if (InData.AttributeType == EExecutionAttributeType::HealthRatio)
			Count = (asc->GetHealth() / asc->GetMaxHealth());
		else if (InData.AttributeType == EExecutionAttributeType::Power)
			Count = asc->GetPower();
		else if (InData.AttributeType == EExecutionAttributeType::Speed)
			Count = asc->GetSpeed();
	}

	// 범위 판정
	if (!bApply)
	{
		if (InData.CompareType == EExecutionCompareType::Less && Count < InData.Threshold)bApply = 1;
		else if (InData.CompareType == EExecutionCompareType::LessOrEqual && Count <= InData.Threshold)bApply = 1;
		else if (InData.CompareType == EExecutionCompareType::Equal && FMath::IsNearlyEqual(Count, InData.Threshold))bApply = 1;
		else if (InData.CompareType == EExecutionCompareType::GreaterOrEqual && InData.Threshold <= Count)bApply = 1;
		else if (InData.CompareType == EExecutionCompareType::Greater && InData.Threshold < Count)bApply = 1;
	}
}

void UExecution_ResolveEffectValue::HandleCamerMoveContext(const FTurnBasedEffectContext* InContext)const
{
	//CheckTrue(!CamerMoveContext.GCNTag.IsValid());
	//CheckTrue(!CamerMoveContext.Payload);

	//ATurnBasedCharacter* ref = Cast<ATurnBasedCharacter>(InContext->EffectCauserActor.Get());;
	//CheckTrue_Print(!ref, "target cast FAILED!!");

	//UAbilityComponent* asc = Cast<UAbilityComponent>(ref->GetAbilitySystemComponent());
	//CheckTrue_Print(!asc, "asc cast FAILED!!");

	//FGameplayCueParameters gameplayCueParameters;
	//FTurnBasedEffectContext* context = CamerMoveContext.Duplicate();
	//context->RuleSourceActor = InContext->EffectSourceActor.Get();
	//context->EventCauserActor = InContext->EffectCauserActor.Get();
	//context->EventTargetActors.Add(InContext->EffectTargetActor.Get());

	//gameplayCueParameters.EffectContext = FGameplayEffectContextHandle(context);
	//asc->ExecuteGameplayCue(CamerMoveContext.GCNTag, gameplayCueParameters);
}
