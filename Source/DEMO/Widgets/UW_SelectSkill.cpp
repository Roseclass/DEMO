#include "Widgets/UW_SelectSkill.h"
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

#include "Datas/UITypes.h"

#include "GameAbilities/AbilityComponent.h"
#include "GameAbilities/GA_Skill.h"

#include "Objects/SelectWidgetActor.h"

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////UUW_SelectSkillIcon//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void UUW_SelectSkillIcon::Refresh(FGameplayTag InTag)
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

void UUW_SelectSkillIcon::OnFocus()
{
	bool bPlaying = IsAnimationPlaying(FocusAnim);
	bool bForward = IsAnimationPlayingForward(FocusAnim);
	float currentTime = 0;
	
	if (bPlaying)
		if(bForward)return;
		else
		{
			currentTime = GetAnimationCurrentTime(FocusAnim);
			PauseAnimation(FocusAnim);
		}

	PlayAnimationAtTime(FocusAnim, currentTime, 1, EUMGSequencePlayMode::Forward);
}

void UUW_SelectSkillIcon::OffFocus()
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
//////////////////////////UUW_SelectSkill//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void UUW_SelectSkill::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	// 위젯이 키 입력을 받을 수 있도록
	this->bIsFocusable = true;
}

void UUW_SelectSkill::NativeConstruct()
{
	Super::NativeConstruct();

	Icons.Add(Icon_Attack);	Icons.Add(Icon_Slot0);
	Icons.Add(Icon_Slot1);	Icons.Add(Icon_Passive);

	Icons[int32(CursorLocation)]->OnFocus();
}

FReply UUW_SelectSkill::NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent)
{
	const FKey Key = InKeyEvent.GetKey();

	if (Key == EKeys::W)
	{
		Up();
		return FReply::Handled();
	}
	else if (Key == EKeys::A)
	{
		return FReply::Handled();
	}
	else if (Key == EKeys::S)
	{
		Down();
		return FReply::Handled();
	}
	else if (Key == EKeys::D)
	{
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

void UUW_SelectSkill::ChangeCursorLocation(int32 Offset)
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

	CheckTrue_Print(!TargetCharacter, "TargetCharacter is nullptr");
	TargetCharacter->OnAnimTagChanged.Broadcast(Icons[int32(CursorLocation)]->GetTag());
}

void UUW_SelectSkill::Up()
{
	// move to up
	ChangeCursorLocation(-1);
}

void UUW_SelectSkill::Down()
{
	// move to left
	ChangeCursorLocation(1);
}

void UUW_SelectSkill::Confirm()
{
	OnConfirmDown.Broadcast(Icons[int32(CursorLocation)]->GetTag());
}

void UUW_SelectSkill::SetWidgetDatas(ATurnBasedCharacter* NewTargetCharacter)
{
	TargetCharacter = NewTargetCharacter;
	CheckTrue_Print(!TargetCharacter, "TargetCharacter is nullptr");

	UAbilityComponent* asc = Cast<UAbilityComponent>(TargetCharacter->GetAbilitySystemComponent());
	CheckTrue_Print(!asc, "asc is nullptr");

	ADEMOPlayerController* pc = Cast<ADEMOPlayerController>(UGameplayStatics::GetPlayerControllerFromID(GetWorld(), 0));
	CheckTrue_Print(!pc, "pc cast Failed");

	for (int32 idx = 0; idx < int32(ESkillSlotLocation::MAX); idx++)
		Icons[idx]->Refresh(TargetCharacter->GetEquippedSkillTags()[idx]);

	TargetCharacter->OnAnimTagChanged.Broadcast(Icons[int32(CursorLocation)]->GetTag());
}

void UUW_SelectSkill::SetOwningActor(ASelectWidgetActor* NewOwningActor)
{
	OwningActor = NewOwningActor;
}

void UUW_SelectSkill::Activate(ATurnBasedCharacter* NewCurrentTurnCharacter)
{
	SetWidgetDatas(NewCurrentTurnCharacter);
	this->SetKeyboardFocus();
}
