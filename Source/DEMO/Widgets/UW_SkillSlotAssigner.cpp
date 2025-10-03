#include "Widgets/UW_SkillSlotAssigner.h"
#include "Global.h"
#include "Blueprint/SlateBlueprintLibrary.h"
#include "Blueprint/WidgetLayoutLibrary.h"

#include "Components/Button.h"

#include "DEMOPlayerController.h"
#include "SaveLoadSubsystem.h"

#include "Datas/UITypes.h"

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////UUW_SkillSlotAssignerSlotSelector////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void UUW_SkillSlotAssignerSlotSelector::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	Button_Attack->OnClicked.AddDynamic(this, &UUW_SkillSlotAssignerSlotSelector::OnAttackClicked);
	Button_Skill_0->OnClicked.AddDynamic(this, &UUW_SkillSlotAssignerSlotSelector::OnSkill_0Clicked);
	Button_Skill_1->OnClicked.AddDynamic(this, &UUW_SkillSlotAssignerSlotSelector::OnSkill_1Clicked);
	Button_Passive->OnClicked.AddDynamic(this, &UUW_SkillSlotAssignerSlotSelector::OnPassiveClicked);

#define INIT_BUTTON(TYPE) \
Buttons[int32(ESkillSlotLocation::##TYPE)] = Button_##TYPE;

	INIT_BUTTON(Attack);
	INIT_BUTTON(Skill_0);
	INIT_BUTTON(Skill_1);
	INIT_BUTTON(Passive);

#undef INIT_BUTTON
}

void UUW_SkillSlotAssignerSlotSelector::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);
}

void UUW_SkillSlotAssignerSlotSelector::OnAttackClicked()
{
	OnButtonClicked.Broadcast(ESkillSlotLocation::Attack, SkillTag);
	Hide();
}

void UUW_SkillSlotAssignerSlotSelector::OnSkill_0Clicked()
{
	OnButtonClicked.Broadcast(ESkillSlotLocation::Skill_0, SkillTag);
	Hide();
}

void UUW_SkillSlotAssignerSlotSelector::OnSkill_1Clicked()
{
	OnButtonClicked.Broadcast(ESkillSlotLocation::Skill_1, SkillTag);
	Hide();
}

void UUW_SkillSlotAssignerSlotSelector::OnPassiveClicked()
{
	OnButtonClicked.Broadcast(ESkillSlotLocation::Passive, SkillTag);
	Hide();
}

void UUW_SkillSlotAssignerSlotSelector::Show(FGameplayTag NewTag)
{
	SkillTag = NewTag;

	ADEMOPlayerController* pc = Cast<ADEMOPlayerController>(GetOwningPlayer());
	CheckTrue_Print(!pc, "pc cast Failed!!!");

	const FDEMOSkillUIData* data = pc->FindUIDataByTag(SkillTag);
	CheckTrue_Print(!data, "data is nullptr");

	for (int32 i = 0; i < int32(ESkillSlotLocation::MAX); i++)
		Buttons[i]->SetVisibility(ESlateVisibility::Collapsed);

	for (auto i : data->AllowedSlotTypes)
		Buttons[int32(i)]->SetVisibility(ESlateVisibility::Visible);

	FVector2D viewport = UWidgetLayoutLibrary::GetViewportSize(GetWorld());
	FVector2D mousePos = UWidgetLayoutLibrary::GetMousePositionOnViewport(GetWorld());
	FVector2D size = GetCachedGeometry().GetLocalSize();

	mousePos.Y = UKismetMathLibrary::FClamp(mousePos.Y, 0, viewport.Y);

	SetRenderTranslation(mousePos);
	GetWorld()->GetTimerManager().SetTimerForNextTick([this]() {SetVisibility(ESlateVisibility::Visible); });	
}

void UUW_SkillSlotAssignerSlotSelector::Hide()
{
	SetVisibility(ESlateVisibility::Hidden);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////UUW_SkillSlotAssignerPopup///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void UUW_SkillSlotAssignerPopup::Refresh()
{
	SkillTag;

	/*
	* 플레이어컨트롤러에서 데이터 뽑아오기
	*/
}

void UUW_SkillSlotAssignerPopup::Show(FGameplayTag NewTag)
{
	if (SkillTag != NewTag)
	{
		SkillTag = NewTag;
		Refresh();
	}
	SetVisibility(ESlateVisibility::Visible);
}

void UUW_SkillSlotAssignerPopup::Hide()
{
	SetVisibility(ESlateVisibility::Hidden);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////UUW_SkillSlotAssignerIcon////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void UUW_SkillSlotAssignerIcon::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	Button->OnClicked.AddDynamic(this, &UUW_SkillSlotAssignerIcon::OnButtonClicked);
}

void UUW_SkillSlotAssignerIcon::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	if (IsHovered())
	{
		FVector2D mousePos;
		APlayerController* pc = GetOwningPlayer();
		if (!pc || !pc->GetMousePosition(mousePos.X, mousePos.Y))return;

		FGeometry geometry = GetCachedGeometry();
		FVector2D local = geometry.AbsoluteToLocal(mousePos);
		USlateBlueprintLibrary::ScreenToWidgetLocal(GetWorld(), geometry, mousePos, local);
		FVector2D size = geometry.GetLocalSize();
		FVector2D uv = local / size;
		uv -= FVector2D(0.5);
		uv *= uv;
		if (uv.X + uv.Y <= 0.25 && !bIsPopupOpen)
		{			
			bIsPopupOpen = 1;
			OnMouseEnter.Broadcast(SkillTag);
		}
		else if(0.25 < uv.X + uv.Y && bIsPopupOpen)
		{
			bIsPopupOpen = 0;
			OnMouseLeave.Broadcast();
		}

		if (uv.X + uv.Y <= 0.25)CLog::Print("IN", -1, 0);
	}

}

void UUW_SkillSlotAssignerIcon::OnButtonClicked()
{
	CheckTrue(SkillTag == FGameplayTag::EmptyTag);
	OnClicked.Broadcast(SkillTag);
}

void UUW_SkillSlotAssignerIcon::Refresh(FGameplayTag NewTag)
{
	SkillTag = NewTag;

	ADEMOPlayerController* pc = Cast<ADEMOPlayerController>(GetOwningPlayer());
	CheckTrue_Print(!pc, "pc cast Failed!!!");

	if (SkillTag == FGameplayTag::EmptyTag)
	{
		UObject* obj = pc->GetEmptyRoundIconResourceObject();
		Button->WidgetStyle.Normal.SetResourceObject(obj);
		Button->WidgetStyle.Hovered.SetResourceObject(obj);
		Button->WidgetStyle.Pressed.SetResourceObject(obj);
		return;
	}

	const FDEMOSkillUIData* data = pc->FindUIDataByTag(SkillTag);
	CheckTrue_Print(!data, "data is nullptr");

	Button->WidgetStyle.Normal.SetResourceObject(data->ResourceObject);
	Button->WidgetStyle.Hovered.SetResourceObject(data->ResourceObject);
	Button->WidgetStyle.Pressed.SetResourceObject(data->ResourceObject);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////UUW_SkillSlotAssigner////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



void UUW_SkillSlotAssigner::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	this->bIsFocusable = true;

	CharacterSkillRootTags.Add(FGameplayTag::RequestGameplayTag("Skill.Gideon"));
	CharacterSkillRootTags.Add(FGameplayTag::RequestGameplayTag("Skill.Revenant"));
	CharacterSkillRootTags.Add(FGameplayTag::RequestGameplayTag("Skill.Terra"));

#define INIT_LISTICON(NUMB) \
ListIcons[int32(ESkillListLocation::Skill_##NUMB)] = ListIcon_##NUMB; \
ListIcons[int32(ESkillListLocation::Skill_##NUMB)]->OnMouseEnter.AddUFunction(this,"OpenDescriptionPopup"); \
ListIcons[int32(ESkillListLocation::Skill_##NUMB)]->OnMouseLeave.AddUFunction(this, "CloseDescriptionPopup"); \
ListIcons[int32(ESkillListLocation::Skill_##NUMB)]->OnClicked.AddUFunction(this, "OpenSlotSelector");

	INIT_LISTICON(00);	INIT_LISTICON(01);
	INIT_LISTICON(10);	INIT_LISTICON(11);
	INIT_LISTICON(20);	INIT_LISTICON(21);
	INIT_LISTICON(30);	INIT_LISTICON(31);

#undef INIT_LISTICON

#define INIT_SLOTICON(TYPE) \
SlotIcons[int32(ESkillSlotLocation::##TYPE)] = SlotIcon_##TYPE; \
SlotIcons[int32(ESkillSlotLocation::##TYPE)]->OnMouseEnter.AddUFunction(this,"OpenDescriptionPopup"); \
SlotIcons[int32(ESkillSlotLocation::##TYPE)]->OnMouseLeave.AddUFunction(this, "CloseDescriptionPopup"); \

	INIT_SLOTICON(Attack);
	INIT_SLOTICON(Skill_0);
	INIT_SLOTICON(Skill_1);
	INIT_SLOTICON(Passive);

#undef INIT_SLOTICON

	SlotSelector->OnButtonClicked.AddUFunction(this, "ChangeSlotData");
}

void UUW_SkillSlotAssigner::NativeConstruct()
{
	Super::NativeConstruct();
	CharacterSkillRootTagIndex = 1;
}

FReply UUW_SkillSlotAssigner::NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent)
{
	const FKey Key = InKeyEvent.GetKey();

	if (Key == EKeys::W)
	{
		return FReply::Handled();
	}
	else if (Key == EKeys::A)
	{
		return FReply::Handled();
	}
	else if (Key == EKeys::S)
	{
		return FReply::Handled();
	}
	else if (Key == EKeys::D)
	{
		return FReply::Handled();
	}
	else if (Key == EKeys::SpaceBar)
	{
		return FReply::Handled();
	}

	// 위젯에서 처리하지 않은 키는 부모에게 전달
	return Super::NativeOnKeyDown(InGeometry, InKeyEvent);
}

void UUW_SkillSlotAssigner::ChangeCurrentCharacterData(bool IsLeft)
{
	CharacterSkillRootTagIndex += IsLeft ? -1 : 1;
	CharacterSkillRootTagIndex += CharacterSkillRootTags.Num();
	CharacterSkillRootTagIndex %= CharacterSkillRootTags.Num();
}

void UUW_SkillSlotAssigner::ChangeSlotData(ESkillSlotLocation InLocation, FGameplayTag InTag)
{
	CachedUISaveDatas[CharacterSkillRootTags[CharacterSkillRootTagIndex]].EquippedSkillTags[int32(InLocation)] = InTag;
	SlotIcons[int32(InLocation)]->Refresh(InTag);

	for (int32 i = 0; i <= int32(ESkillSlotLocation::MAX); i++)
	{
		if (i == int32(InLocation))continue;
		if (SlotIcons[i]->GetSkillTag() == InTag)
		{
			SlotIcons[i]->Refresh(FGameplayTag::EmptyTag);
		}
	}
}

void UUW_SkillSlotAssigner::OpenDescriptionPopup(FGameplayTag InTag)
{
	DescriptionPopup->Show(InTag);
}

void UUW_SkillSlotAssigner::CloseDescriptionPopup()
{
	DescriptionPopup->Hide();
}

void UUW_SkillSlotAssigner::OpenSlotSelector(FGameplayTag InTag)
{
	SlotSelector->Show(InTag);
}

void UUW_SkillSlotAssigner::CloseSlotSelector()
{
	SlotSelector->Hide();
}

void UUW_SkillSlotAssigner::Refresh(const FDEMOSkillUIDatas* Datas)
{
	for (int32 i = 0; i < int32(ESkillListLocation::MAX); i++)
		ListIcons[i]->Refresh(Datas->Datas[i].SkillTag);
}

void UUW_SkillSlotAssigner::UpdateCachedUISaveDatas()
{
	USaveLoadSubsystem* SLS = GetGameInstance()->GetSubsystem<USaveLoadSubsystem>();
	for (auto& i : CachedUISaveDatas)
		SLS->UpdateUISaveData(i.Key, i.Value);
	CachedUISaveDatas.Empty();
}

void UUW_SkillSlotAssigner::Init(AActor* Manager, AActor* Camera)
{
	// 매니저와 카메라를 링크
}

void UUW_SkillSlotAssigner::Show()
{
	// 월드 멈춤

	// 화면 꺼짐

	// 카메라로 시점 이동

	// 화면 켜짐 + 위젯보임

	USaveLoadSubsystem* SLS = GetGameInstance()->GetSubsystem<USaveLoadSubsystem>();
	CachedUISaveDatas = SLS->ReadGameData()->SavedPlayerUIDatas;

	if (!CharacterSkillRootTags[CharacterSkillRootTagIndex].IsValid())
		CharacterSkillRootTags[CharacterSkillRootTagIndex] = CachedUISaveDatas.begin()->Key;

	ADEMOPlayerController* pc = Cast<ADEMOPlayerController>(GetOwningPlayer());
	CheckTrue_Print(!pc, "pc cast Failed!!!");

	const FDEMOSkillUIDatas* datas = pc->FindUIDatasByTag(CharacterSkillRootTags[CharacterSkillRootTagIndex]);
	CheckTrue_Print(!datas, "datas is nullptr");
	Refresh(datas);

	SetVisibility(ESlateVisibility::Visible);
	SetFocus();
}

void UUW_SkillSlotAssigner::Hide()
{
	// 화면 꺼짐

	// TPS 캐릭터로 시점 이동

	// 화면 켜짐 + 월드 복구

	UpdateCachedUISaveDatas();
	SetVisibility(ESlateVisibility::Collapsed);
}

/*
* 
* 캐릭터 변경마다 temp에서 
* 
*/