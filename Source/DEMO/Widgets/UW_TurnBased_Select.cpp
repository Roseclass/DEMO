#include "Widgets/UW_TurnBased_Select.h"
#include "Global.h"

#include "Animation/WidgetAnimation.h"
#include "Animation/UMGSequencePlayer.h"

#include "Components/Image.h"
#include "Components/ScaleBox.h"
#include "Components/TextBlock.h"
#include "Components/RichTextBlock.h"
#include "Components/WidgetComponent.h"

#include "DEMOPlayerController.h"

#include "Characters/TurnBasedCharacter.h"
#include "Characters/AI/TurnBasedEnemy.h"

#include "Datas/UITypes.h"

#include "GameAbilities/AbilityComponent.h"
#include "GameAbilities/GA_Skill.h"

#include "Objects/SelectWidgetActor.h"

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////UUW_TurnBased_SelectSkillRow/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void UUW_TurnBased_SelectSkillRow::Refresh(FGameplayTag InTag)
{
	SkillTag = InTag;

	ADEMOPlayerController* pc = Cast<ADEMOPlayerController>(GetOwningPlayer());
	CheckTrue_Print(!pc, "pc cast Failed!!!");

	if (SkillTag == FGameplayTag::EmptyTag)
	{
		UObject* obj = pc->GetEmptyRoundIconResourceObject();
		Cover->SetBrushResourceObject(obj);
		return;
	}

	const FDEMOSkillUIData* data = pc->FindUIDataByTag(SkillTag);
	if (!data)
	{
		UObject* obj = pc->GetEmptyRoundIconResourceObject();
		Cover->SetBrushResourceObject(obj);
	}
	CheckTrue_Print(!data, "data is nullptr");

	Cover->SetBrushResourceObject(data->ResourceObject);
}

void UUW_TurnBased_SelectSkillRow::OnFocus()
{
	bool bPlaying = IsAnimationPlaying(FocusAnim);
	bool bForward = IsAnimationPlayingForward(FocusAnim);
	float currentTime = 0;

	if (bPlaying)
		if (bForward)return;
		else
		{
			currentTime = GetAnimationCurrentTime(FocusAnim);
			PauseAnimation(FocusAnim);
		}

	PlayAnimationAtTime(FocusAnim, currentTime, 1, EUMGSequencePlayMode::Forward);
}

void UUW_TurnBased_SelectSkillRow::OffFocus()
{
	bool bPlaying = IsAnimationPlaying(FocusAnim);
	bool bBackward = !IsAnimationPlayingForward(FocusAnim);
	float currentTime = 0;

	if (bPlaying)
		if (bBackward)return;
		else
		{
			currentTime = FocusAnim->GetEndTime() - GetAnimationCurrentTime(FocusAnim);
			PauseAnimation(FocusAnim);
		}

	PlayAnimationAtTime(FocusAnim, currentTime, 1, EUMGSequencePlayMode::Reverse);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////UUW_TurnBased_Select/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void UUW_TurnBased_Select::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	// 위젯이 키 입력을 받을 수 있도록
	this->bIsFocusable = true;
}

void UUW_TurnBased_Select::NativeConstruct()
{
	Super::NativeConstruct();

	Icons.Add(Icon_Attack);	Icons.Add(Icon_Slot0);
	Icons.Add(Icon_Slot1);	Icons.Add(Icon_Passive);

	Icons[int32(CursorLocation)]->OnFocus();
}

FReply UUW_TurnBased_Select::NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent)
{
	if (!CanAcceptKey.Execute())
		return Super::NativeOnKeyDown(InGeometry, InKeyEvent);

	const FKey Key = InKeyEvent.GetKey();

	if (Key == EKeys::W)
	{
		Up();
		return FReply::Handled();
	}
	else if (Key == EKeys::A)
	{
		Left();
		return FReply::Handled();
	}
	else if (Key == EKeys::S)
	{
		Down();
		return FReply::Handled();
	}
	else if (Key == EKeys::D)
	{
		Right();
		return FReply::Handled();
	}
	else if (Key == EKeys::SpaceBar)
	{
		Confirm();
		return FReply::Handled();
	}

	// 위젯에서 처리하지 않은 키는 부모에게 전달
	return Super::NativeOnKeyDown(InGeometry, InKeyEvent);
}

void UUW_TurnBased_Select::ChangeCursorLocation(int32 Offset)
{
	//기존위치 off
	Icons[int32(CursorLocation)]->OffFocus();

	//위치계산
	int32 mod = int32(ESkillSlotLocation::MAX);
	int32 cur = int32(CursorLocation) + Offset + mod;
	cur %= mod;
	CursorLocation = ESkillSlotLocation(cur);

	//새로운 위치 on
	Icons[int32(CursorLocation)]->OnFocus();

	CheckTrue_Print(!TurnCharacter, "TurnCharacter is nullptr");
	TurnCharacter->OnAnimTagChanged.Broadcast(Icons[int32(CursorLocation)]->GetTag());
}

void UUW_TurnBased_Select::Up()
{
	// move to up
	ChangeCursorLocation(-1);
}

void UUW_TurnBased_Select::Down()
{
	// move to left
	ChangeCursorLocation(1);
}

void UUW_TurnBased_Select::Left()
{
	TargetIndex--;
	TargetIndex = (TargetIndex + TargetArray.Num()) % TargetArray.Num();
	TargetCursorActor->SetActorTransform(TargetArray[TargetIndex]->GetSelectTargetTransform());
	OnHorizontalMove.Broadcast(TargetArray[TargetIndex]);
}

void UUW_TurnBased_Select::Right()
{
	TargetIndex++;
	TargetIndex %= TargetArray.Num();
	TargetCursorActor->SetActorTransform(TargetArray[TargetIndex]->GetSelectTargetTransform());
	OnHorizontalMove.Broadcast(TargetArray[TargetIndex]);
}

void UUW_TurnBased_Select::Confirm()
{
	OnConfirm.Broadcast(Icons[int32(CursorLocation)]->GetTag(), TargetArray[TargetIndex]);
}

void UUW_TurnBased_Select::SetWidgetDatas(ATurnBasedCharacter* NewTurnCharacter)
{
	ATurnBasedEnemy* enemy = Cast<ATurnBasedEnemy>(TurnCharacter);
	if (enemy)
	{
		//enemy->LinkSelectSkill(nullptr);
		//enemy->OnChangeCursorLocation.Unbind();
		//enemy->OnConfirm.Unbind();
	}

	TurnCharacter = NewTurnCharacter;
	CheckTrue_Print(!TurnCharacter, "TurnCharacter is nullptr");

	for (int32 idx = 0; idx < int32(ESkillSlotLocation::MAX); idx++)
		Icons[idx]->Refresh(TurnCharacter->GetEquippedSkillTags()[idx]);

	TurnCharacter->OnAnimTagChanged.Broadcast(Icons[int32(CursorLocation)]->GetTag());

	enemy = Cast<ATurnBasedEnemy>(TurnCharacter);
	if (enemy)
	{
		//enemy->LinkSelectSkill(this);
		//enemy->OnChangeCursorLocation.BindUFunction(this, "ChangeCursorLocation");
		//enemy->OnConfirm.BindUFunction(this, "Confirm");
	}
}

void UUW_TurnBased_Select::SetOwningActor(ASelectWidgetActor* NewOwningActor)
{
	OwningActor = NewOwningActor;
}

void UUW_TurnBased_Select::SetTargetCursorActor(ASelectWidgetActor* NewTargetCursorActor)
{
	TargetCursorActor = NewTargetCursorActor;
}

void UUW_TurnBased_Select::Activate(ATurnBasedCharacter* NewCurrentTurnCharacter, const TArray<ATurnBasedCharacter*>& InArray)
{
	SetWidgetDatas(NewCurrentTurnCharacter);

	TargetIndex = 0;
	TargetArray = InArray;
	this->SetKeyboardFocus();

	OnHorizontalMove.Broadcast(TargetArray[TargetIndex]);
}

FGameplayTag UUW_TurnBased_Select::GetCurrentSkillTag() const
{
	return Icons[int32(CursorLocation)]->GetTag();
}

TArray<FGameplayTag> UUW_TurnBased_Select::GetAllSkillTags() const
{
	TArray<FGameplayTag> result;
	for (auto icon : Icons)
		result.Add(icon->GetTag());
	return result;
}
