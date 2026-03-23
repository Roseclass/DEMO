#include "Widgets/UW_TBSelect.h"
#include "Global.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Components/HorizontalBox.h"

#include "DEMOPlayerController.h"

#include "Characters/TurnBasedCharacter.h"
#include "Characters/AI/TurnBasedEnemy.h"

#include "GameAbilities/AbilityComponent.h"
#include "GameAbilities/GA_Skill.h"

#include "Datas/UITypes.h"

void UUW_TBSelect_SkillIcon::NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	Super::NativeOnMouseEnter(InGeometry, InMouseEvent);
	OnSkillIconMouseEnter.ExecuteIfBound(SkillTag);
	// 스킬 아이콘 호버:툴팁ON currenttag갱신
}

void UUW_TBSelect_SkillIcon::NativeOnMouseLeave(const FPointerEvent& InMouseEvent)
{
	Super::NativeOnMouseLeave(InMouseEvent);
	OnSkillIconMouseLeave.ExecuteIfBound(FGameplayTag());
	// 스킬 아이콘 호버:툴팁Off currenttag갱신
}

FReply UUW_TBSelect_SkillIcon::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	if(InMouseEvent.GetEffectingButton() == EKeys::LeftMouseButton)
		OnSkillIconClicked.ExecuteIfBound();

	return Super::NativeOnMouseButtonDown(InGeometry, InMouseEvent);
}

void UUW_TBSelect_SkillIcon::Refresh(FGameplayTag NewSkillTag, float NewTimeRemaining, float NewCooldownDuration)
{
	SkillTag = NewSkillTag;
	TimeRemaining = NewTimeRemaining;
	CooldownDuration = NewCooldownDuration;

	ADEMOPlayerController* pc = Cast<ADEMOPlayerController>(GetOwningPlayer());
	CheckTrue_Print(!pc, "pc cast Failed!!!");

	if (SkillTag == FGameplayTag::EmptyTag)
	{
		SetVisibility(ESlateVisibility::Collapsed);
		return;
	}

	const FDEMOSkillUIData* data = pc->FindUIDataByTag(SkillTag);
	if (!data)SetVisibility(ESlateVisibility::Collapsed);
	CheckTrue_Print(!data, "data is nullptr");
	SkillIcon->SetBrushResourceObject(data->ResourceObject);

	
	if (TimeRemaining < 0)TimeRemaining = 0;
	if (CooldownDuration <= 0)CooldownDuration = 1;
	float rate = (CooldownDuration - TimeRemaining) / (CooldownDuration);
	CooldownRateCover->GetDynamicMaterial()->SetScalarParameterValue(FName("Rate"), rate);

	FString cdText = "";
	if (0 < TimeRemaining) cdText = FString::FromInt(TimeRemaining);
	TimeRemainingText->SetText(FText::FromString(cdText));
}

/////////////////////////////////////////////////////////////////////////////////////////////
////////////////////UUW_TBSelect/////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////

void UUW_TBSelect::NativeConstruct()
{
	Super::NativeConstruct();
}

void UUW_TBSelect::CreateIcon()
{
	CheckTrue_Print(!SkillIconClass, "SkillIconClass is nullptr");

	UUW_TBSelect_SkillIcon* icon = CreateWidget<UUW_TBSelect_SkillIcon>(GetOwningPlayer(), SkillIconClass);
	icon->OnSkillIconMouseEnter.BindUFunction(this, "ExecuteOnSkillIconMouseEnter");
	icon->OnSkillIconMouseLeave.BindUFunction(this, "ExecuteOnSkillIconMouseLeave");
	icon->OnSkillIconClicked.BindUFunction(this, "ExecuteOnSkillIconClicked");

	SkillIconContainer->AddChildToHorizontalBox(icon);

	Icons.Add(icon);
}

void UUW_TBSelect::ExecuteOnSkillIconMouseEnter(FGameplayTag InSkillTag)
{
	OnSkillIconMouseEnter.ExecuteIfBound(InSkillTag);
}

void UUW_TBSelect::ExecuteOnSkillIconMouseLeave(FGameplayTag InSkillTag)
{
	OnSkillIconMouseLeave.ExecuteIfBound(InSkillTag);
}

void UUW_TBSelect::ExecuteOnSkillIconClicked()
{
	OnSkillIconClicked.ExecuteIfBound();
}

void UUW_TBSelect::Activate(ATurnBasedCharacter* NewTurnCharacter)
{
	GetOwningPlayer()->SetShowMouseCursor(1);

	ATurnBasedEnemy* enemy = Cast<ATurnBasedEnemy>(TurnCharacter);
	if (enemy)
	{
		//enemy->LinkSelectWidget(nullptr);
		//enemy->OnLeft.Unbind();
		//enemy->OnRight.Unbind();
		//enemy->OnUp.Unbind();
		//enemy->OnDown.Unbind();
		//enemy->OnConfirm.Unbind();
	}

	TurnCharacter = NewTurnCharacter;
	CheckTrue_Print(!TurnCharacter, "TurnCharacter is nullptr");

	UAbilityComponent* asc = Cast<UAbilityComponent>(TurnCharacter->GetAbilitySystemComponent());
	TArray<FGameplayTag> equippedSkillTags = TurnCharacter->GetEquippedSkillTags();

	for (int32 idx = 0; idx < FMath::Max(equippedSkillTags.Num(),Icons.Num()); idx++)
	{
		if (!Icons.IsValidIndex(idx))
			CreateIcon();

		if (!equippedSkillTags.IsValidIndex(idx))
		{
			Icons[idx]->SetVisibility(ESlateVisibility::Collapsed);
			continue;
		}

		float remain = 0;
		float cd = 1;

		asc->GetSkillCooldownTimeRemainingAndDuration(equippedSkillTags[idx], remain, cd);
		Icons[idx]->Refresh(equippedSkillTags[idx], remain, cd);
	}

	ExecuteOnSkillIconMouseLeave(FGameplayTag());

	enemy = Cast<ATurnBasedEnemy>(TurnCharacter);
	if (enemy)
	{
		////enemy->LinkSelectWidget(this);
		////enemy->OnLeft.BindUFunction(this, "Left");
		////enemy->OnRight.BindUFunction(this, "Right");
		////enemy->OnUp.BindUFunction(this, "Up");
		////enemy->OnDown.BindUFunction(this, "Down");
		////enemy->OnConfirm.BindUFunction(this, "Confirm");
	}

}
