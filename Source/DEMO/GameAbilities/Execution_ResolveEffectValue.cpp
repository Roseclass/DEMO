#include "GameAbilities/Execution_ResolveEffectValue.h"
#include "Global.h"

#include "Characters/TurnBasedCharacter.h"

#include "GameAbilities/AbilityComponent.h"
#include "GameAbilities/AttributeSet_Character.h"

UExecution_ResolveEffectValue::UExecution_ResolveEffectValue()
{
	UDataTable* ptr;
	CHelpers::GetAsset<UDataTable>(&ptr, "DataTable'/Game/Datas/DT_ExecutionRules.DT_ExecutionRules'");
	
	ResolveRules = ptr;
}

void UExecution_ResolveEffectValue::Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams, FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const
{
	Super::Execute_Implementation(ExecutionParams, OutExecutionOutput);

	const FExecutionContext* context = static_cast<const FExecutionContext*>(ExecutionParams.GetOwningSpec().GetEffectContext().Get());
	CheckTrue_Print(!context, "context cast FAILED!!");

	float tempAdditive = 0;
	float tempMultiplier = 1;

	for (const auto& data : ModifyDatas)
		HandleModifyData(context, data, tempAdditive, tempMultiplier);

	HandleResolveRules(context, tempAdditive, tempMultiplier);

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
}

void UExecution_ResolveEffectValue::HandleResolveRules(const FExecutionContext* InContext, OUT float& Additive, OUT float& Multiplier)const
{
	TArray<FExecutionModifyRule*> arr;
	ResolveRules->GetAllRows("", arr);
	for (auto rule : arr)
	{
		if (rule->ResolveType != ResolveType)continue;
		HandleModifyData(InContext, rule->ResolveData, Additive, Multiplier);
	}
}

void UExecution_ResolveEffectValue::HandleModifyData(const FExecutionContext* InContext, const FExecutionModifyData& InData, OUT float& Additive, OUT float& Multiplier) const
{
	ATurnBasedCharacter* ref = nullptr;

	// 레퍼런스 액터 설정
	switch (InData.ReferenceActor)
	{
	case EExecutionReferenceActorType::EffectSource:ref = Cast<ATurnBasedCharacter>(InContext->EffectSourceActor.Get()); break;
	case EExecutionReferenceActorType::EffectTarget:ref = Cast<ATurnBasedCharacter>(InContext->EffectTargetActor.Get()); break;
	case EExecutionReferenceActorType::EffectCauser:ref = Cast<ATurnBasedCharacter>(InContext->EffectCauserActor.Get()); break;
	case EExecutionReferenceActorType::SkillTarget:ref = Cast<ATurnBasedCharacter>(InContext->SkillTargetActor.Get()); break;
	case EExecutionReferenceActorType::SkillCauser:ref = Cast<ATurnBasedCharacter>(InContext->SkillCauserActor.Get()); break;
	default:break;
	}
	CheckTrue_Print(!ref, "target cast FAILED!!");

	UAbilityComponent* asc = Cast<UAbilityComponent>(ref->GetAbilitySystemComponent());
	CheckTrue_Print(!asc, "asc cast FAILED!!");

	bool bApply = 0;
	float cnt = 0;
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
				if (tag.MatchesTag(InData.Tag))cnt += 1;
		} 
		else if(InData.MeasureType == EExecutionMeasureType::HasTagExact)
		{
			if (tags.HasAnyExact(FGameplayTagContainer(InData.Tag)))
				bApply = 1;
		}
	}
	else if (InData.MeasureType == EExecutionMeasureType::Attribute)
	{
		// 어트리뷰트 값 불러오기
		if (InData.AttributeType == EExecutionAttributeType::Health)
			cnt = asc->GetHealth();
		else if (InData.AttributeType == EExecutionAttributeType::HealthRatio)
			cnt = (asc->GetHealth() / asc->GetMaxHealth());
		else if (InData.AttributeType == EExecutionAttributeType::Power)
			cnt = asc->GetPower();
		else if (InData.AttributeType == EExecutionAttributeType::Speed)
			cnt = asc->GetSpeed();
	}

	// 범위 판정
	if (!bApply)
	{
		if(InData.CompareType == EExecutionCompareType::Less && cnt < InData.Threshold)bApply = 1;
		else if(InData.CompareType == EExecutionCompareType::LessOrEqual && cnt <= InData.Threshold)bApply = 1;
		else if(InData.CompareType == EExecutionCompareType::Equal && FMath::IsNearlyEqual(cnt, InData.Threshold))bApply = 1;
		else if(InData.CompareType == EExecutionCompareType::GreaterOrEqual && InData.Threshold <= cnt)bApply = 1;
		else if(InData.CompareType == EExecutionCompareType::Greater && InData.Threshold < cnt)bApply = 1;
	}

	CheckTrue(!bApply);

	switch (InData.ModifyOp)
	{
	case EExecutionModifyOp::AddConstant:Additive += InData.Value; break;
	case EExecutionModifyOp::AddMeasuredValueScaled:Additive += (cnt * InData.Value); break;
	case EExecutionModifyOp::MultiplyConstant:Additive *= InData.Value; break;
	case EExecutionModifyOp::MultiplyFromMeasuredValue:Multiplier *= (cnt * InData.Value); break;
	default:break;
	}
}
