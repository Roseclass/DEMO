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

void UUW_TurnBased_SelectSkillRow::Refresh(FSelectSkillRowData InData)
{
	RowData = InData;

	ADEMOPlayerController* pc = Cast<ADEMOPlayerController>(GetOwningPlayer());
	CheckTrue_Print(!pc, "pc cast Failed!!!");

	if (RowData.SkillTag == FGameplayTag::EmptyTag)
	{
		UObject* obj = pc->GetEmptyRoundIconResourceObject();
		Cover->SetBrushResourceObject(obj);
		return;
	}

	const FDEMOSkillUIData* data = pc->FindUIDataByTag(RowData.SkillTag);
	if (!data)
	{
		UObject* obj = pc->GetEmptyRoundIconResourceObject();
		Cover->SetBrushResourceObject(obj);
	}
	CheckTrue_Print(!data, "data is nullptr");

	Cover->SetBrushResourceObject(data->ResourceObject);

	bIsCoolDown = 0 < RowData.RemainCd;
	if (RowData.RemainCd < 0)RowData.RemainCd = 0;
	if (RowData.Cd <= 0)RowData.Cd = 1;
	float rate = float(RowData.Cd - RowData.RemainCd) / float(RowData.Cd);
	CooltimeCover->GetDynamicMaterial()->SetScalarParameterValue(FName("Rate"), rate);

	FString cdText = "";
	if (0 < RowData.RemainCd) cdText = FString::FromInt(RowData.RemainCd);
	CooltimeLeft->SetText(FText::FromString(cdText));

	return;
	//debug
	CLog::Print(RowData.SkillTag.ToString() + ", Remain:" + FString::FromInt(RowData.RemainCd) + ", Cd:" + FString::FromInt(RowData.Cd));
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

void UUW_TurnBased_SelectSkillRow::OnFocusWithoutAnim()
{
	ScaleBox->SetUserSpecifiedScale(1);
}

void UUW_TurnBased_SelectSkillRow::OffFocusWithoutAnim()
{
	ScaleBox->SetUserSpecifiedScale(0.7);
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

void UUW_TurnBased_Select::ChangeCursorLocation(ESkillSlotLocation NewLocation, bool Anim)
{
	//기존위치 off
	Anim ? Icons[int32(CursorLocation)]->OffFocus() : Icons[int32(CursorLocation)]->OffFocusWithoutAnim();

	//새로운 위치 on
	CursorLocation = NewLocation;
	Anim ? Icons[int32(CursorLocation)]->OnFocus() : Icons[int32(CursorLocation)]->OnFocusWithoutAnim();

	CheckTrue_Print(!TurnCharacter, "TurnCharacter is nullptr");
	TurnCharacter->OnAnimTagChanged.Broadcast(Icons[int32(CursorLocation)]->GetSkillTag());
}

void UUW_TurnBased_Select::Up()
{
	//위치계산
	int32 mod = int32(ESkillSlotLocation::MAX);
	int32 cur = int32(CursorLocation) - 1 + mod;
	cur %= mod;
	ChangeCursorLocation(ESkillSlotLocation(cur));
}

void UUW_TurnBased_Select::Down()
{
	//위치계산
	int32 mod = int32(ESkillSlotLocation::MAX);
	int32 cur = int32(CursorLocation) + 1 + mod;
	cur %= mod;
	ChangeCursorLocation(ESkillSlotLocation(cur));
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
	CheckTrue(Icons[int32(CursorLocation)]->IsCoolDown());
	CheckTrue(!CheckTargetType());

	OnConfirm.Broadcast(Icons[int32(CursorLocation)]->GetSkillTag(), TargetArray[TargetIndex]);
}

bool UUW_TurnBased_Select::CheckTargetType()
{
	//사용가능1 <-> 불가능0

	ESkillTargetType type = Icons[int32(CursorLocation)]->GetRowData().TargetType;	
	if (type == ESkillTargetType::Enemy &&
		TurnCharacter->GetGenericTeamId() == TargetArray[TargetIndex]->GetGenericTeamId())
		return 0;
	else if (type == ESkillTargetType::Ally &&
		TurnCharacter->GetGenericTeamId() != TargetArray[TargetIndex]->GetGenericTeamId())
		return 0;
	else if (type == ESkillTargetType::AllyButSelf)
	{
		if(TurnCharacter->GetGenericTeamId() != TargetArray[TargetIndex]->GetGenericTeamId())return 0;
		if(TurnCharacter == TargetArray[TargetIndex])return 0;
	}
	else if (type == ESkillTargetType::AllButSelf &&
		TurnCharacter == TargetArray[TargetIndex])
		return 0;
	else if (type == ESkillTargetType::Self &&
		TurnCharacter != TargetArray[TargetIndex])
		return 0;
	return 1;
}

void UUW_TurnBased_Select::SetWidgetDatas(ATurnBasedCharacter* NewTurnCharacter)
{
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
	TArray<FGameplayAbilitySpec> specs = asc->GetActivatableAbilities();
	for (int32 idx = 0; idx < int32(ESkillSlotLocation::MAX); idx++)
	{
		float remain = 0;
		float cd = 1;
		const UGA_Skill* skill = nullptr;
		for (auto& spec : specs)
		{
			skill = Cast<UGA_Skill>(spec.Ability);
			if(!skill)continue;
			if (TurnCharacter->GetEquippedSkillTags()[idx] != skill->GetSkillTag())continue;
			skill->GetCooldownTimeRemainingAndDuration(spec.Handle, asc->AbilityActorInfo.Get(), remain, cd);
			break;
		}
		if (!skill)continue;
		Icons[idx]->Refresh(
			FSelectSkillRowData(TurnCharacter->GetEquippedSkillTags()[idx], skill->GetTargetType(), remain, cd)
			);
	}

	TurnCharacter->OnAnimTagChanged.Broadcast(Icons[int32(CursorLocation)]->GetSkillTag());
	ChangeCursorLocation(ESkillSlotLocation::Attack, 0);

	enemy = Cast<ATurnBasedEnemy>(TurnCharacter);
	if (enemy)
	{
		//enemy->LinkSelectWidget(this);
		//enemy->OnLeft.BindUFunction(this, "Left");
		//enemy->OnRight.BindUFunction(this, "Right");
		//enemy->OnUp.BindUFunction(this, "Up");
		//enemy->OnDown.BindUFunction(this, "Down");
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

void UUW_TurnBased_Select::Activate(ATurnBasedCharacter* NewCurrentTurnCharacter, const TArray<ATurnBasedCharacter*>& InPlayerArray, const TArray<ATurnBasedCharacter*>& InEnemyArray)
{
	SetWidgetDatas(NewCurrentTurnCharacter);

	TargetArray.Empty();
	PlayerArray.Empty();
	EnemyArray.Empty();

	PlayerStartIndex = 0;
	for(auto ch : InPlayerArray)
	{
		if (ch->IsDead())continue;
		TargetArray.Add(ch);
		PlayerArray.Add(ch);
	}

	EnemyStartIndex = TargetArray.Num();
	for(auto ch : InEnemyArray)
	{
		if (ch->IsDead())continue;
		TargetArray.Add(ch);
		EnemyArray.Add(ch);
	}

	TargetIndex = TurnCharacter->GetGenericTeamId() == TEAMID_PLAYER ? EnemyStartIndex : PlayerStartIndex;
	this->SetKeyboardFocus();

	TargetCursorActor->SetActorTransform(TargetArray[TargetIndex]->GetSelectTargetTransform());
	OnHorizontalMove.Broadcast(TargetArray[TargetIndex]);
}

FGameplayTag UUW_TurnBased_Select::GetCurrentSkillTag() const
{
	return Icons[int32(CursorLocation)]->GetSkillTag();
}

TArray<FGameplayTag> UUW_TurnBased_Select::GetAllSkillTags() const
{
	TArray<FGameplayTag> result;
	for (auto icon : Icons)
		result.Add(icon->GetSkillTag());
	return result;
}

ATurnBasedCharacter* UUW_TurnBased_Select::GetCurrentTarget()const
{
	return TargetArray[TargetIndex];
}

TArray<ATurnBasedCharacter*> UUW_TurnBased_Select::GetTargetArray()const
{
	return TargetArray;
}

TArray<ATurnBasedCharacter*> UUW_TurnBased_Select::GetPlayerArray()const
{
	return PlayerArray;
}

TArray<ATurnBasedCharacter*> UUW_TurnBased_Select::GetEnemyArray()const
{
	return EnemyArray;
}
