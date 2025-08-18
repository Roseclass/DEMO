#include "Widgets/UW_MainMenu_SaveDataSelect.h"
#include "Global.h"

#include "Components/ListView.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"

#include "Widgets/UW_MainMenu_Confirm.h"


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////SaveDataSelect_ListEntry/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void UUW_MainMenu_SaveDataSelect_ListEntry::NativeOnListItemObjectSet(UObject* ListItemObject)
{
	if (UMainMenu_SaveDataSelect_ListData* Data = Cast<UMainMenu_SaveDataSelect_ListData>(ListItemObject))
	{
		if (SaveDataText)
			SaveDataText->SetText(Data->SaveDataText);
	}
	ApplySelectionGlow();
}

void UUW_MainMenu_SaveDataSelect_ListEntry::NativeOnItemSelectionChanged(bool bIsSelected)
{
	ApplySelectionGlow();
}

void UUW_MainMenu_SaveDataSelect_ListEntry::ApplySelectionGlow()
{
	FSlateColor selected(FLinearColor(1.f, 1.f, 1.f, OpacityWhenSelected));
	FSlateColor notSelected(FLinearColor(1.f, 1.f, 1.f, OpacityWhenNotSelected));

	BackgroundImage->SetBrushTintColor(IsListItemSelected() ? selected : notSelected);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////SaveDataSelect///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void UUW_MainMenu_SaveDataSelect::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	// 위젯이 키 입력을 받을 수 있도록
	this->bIsFocusable = true;
}

void UUW_MainMenu_SaveDataSelect::NativeConstruct()
{
	Super::NativeConstruct();

	{
		UMainMenu_SaveDataSelect_ListData* obj = NewObject<UMainMenu_SaveDataSelect_ListData>();
		obj->SaveDataText = FText::FromString(TEXT("Slot1"));
		SaveDataList->AddItem(obj); 
	}
	{
		UMainMenu_SaveDataSelect_ListData* obj = NewObject<UMainMenu_SaveDataSelect_ListData>();
		obj->SaveDataText = FText::FromString(TEXT("Slot2"));
		SaveDataList->AddItem(obj); 
	}
	{
		UMainMenu_SaveDataSelect_ListData* obj = NewObject<UMainMenu_SaveDataSelect_ListData>();
		obj->SaveDataText = FText::FromString(TEXT("Slot3"));
		SaveDataList->AddItem(obj); 
	}
	{
		UMainMenu_SaveDataSelect_ListData* obj = NewObject<UMainMenu_SaveDataSelect_ListData>();
		obj->SaveDataText = FText::FromString(TEXT("Slot4"));
		SaveDataList->AddItem(obj); 
	}
	{
		UMainMenu_SaveDataSelect_ListData* obj = NewObject<UMainMenu_SaveDataSelect_ListData>();
		obj->SaveDataText = FText::FromString(TEXT("Slot5"));
		SaveDataList->AddItem(obj); 
	}
	{
		UMainMenu_SaveDataSelect_ListData* obj = NewObject<UMainMenu_SaveDataSelect_ListData>();
		obj->SaveDataText = FText::FromString(TEXT("Slot6"));
		SaveDataList->AddItem(obj); 
	}

}

FReply UUW_MainMenu_SaveDataSelect::NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent)
{
	const FKey Key = InKeyEvent.GetKey();

	CLog::Print(FString::Printf(TEXT("%s %s"), *FString(__FUNCTION__), *FString::FromInt(__LINE__)));

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

void UUW_MainMenu_SaveDataSelect::Up()
{
	int32 prev = SlotIndex;
	SlotIndex--;

	if (SlotIndex < 0)SlotIndex = SaveDataList->GetNumItems();

	if (SaveDataList->GetListItems().IsValidIndex(SlotIndex))SaveDataList->SetScrollOffset(SlotIndex* 1.078125);

	Glow(prev, SlotIndex);
}

void UUW_MainMenu_SaveDataSelect::Down()
{
	int32 prev = SlotIndex;
	SlotIndex++;

	if (SaveDataList->GetNumItems() < SlotIndex)SlotIndex = 0;

	if (SaveDataList->GetListItems().IsValidIndex(SlotIndex))SaveDataList->SetScrollOffset(SlotIndex* 1.078125);

	Glow(prev,SlotIndex);
}

void UUW_MainMenu_SaveDataSelect::Left()
{
	PrintLine()
}

void UUW_MainMenu_SaveDataSelect::Right()
{
	PrintLine()
}

void UUW_MainMenu_SaveDataSelect::Confirm()
{
	CheckTrue_Print(!ConfirmWidget, "ConfirmWidget is nullptr");
	
	if (SlotIndex == SaveDataList->GetNumItems())
	{
		OnBackConfirmed.Broadcast();
		return;
	}

	if (Type == EMainMenuPhase::NewGame)
	{
		//UNDONE 세이브로드와 연결하기

		//if(saveslotisalreadyoccupied)
		ConfirmWidget->Show(
			Type,
			[=]()
			{
				this->Save(SlotIndex);
			},
			[=]() 
			{
				this->SetFocus();
			});
		//else
		Save(SlotIndex);
	}
	else if (Type == EMainMenuPhase::LoadGame)
	{
		Load(SlotIndex);
	}
	else CheckTrue_Print(1, "Type is not Valid");
}

void UUW_MainMenu_SaveDataSelect::Glow(int32 PrevIdx, int32 CurIdx)
{
	FSlateColor selected(FLinearColor(1.f, 1.f, 1.f, OpacityWhenSelected));
	FSlateColor notSelected(FLinearColor(1.f, 1.f, 1.f, OpacityWhenNotSelected));	

	Back_Background->SetBrushTintColor(SlotIndex == SaveDataList->GetNumItems() ? selected : notSelected);

	if (SaveDataList->GetListItems().IsValidIndex(PrevIdx))
		SaveDataList->SetItemSelection(SaveDataList->GetItemAt(PrevIdx), 0);

	if (SaveDataList->GetListItems().IsValidIndex(CurIdx))
		SaveDataList->SetItemSelection(SaveDataList->GetItemAt(CurIdx), 1);
}

void UUW_MainMenu_SaveDataSelect::Save(int32 InSlotIndex)
{
	//UNDONE 세이브로드와 연결하기
	CLog::Print(InSlotIndex);
	CLog::Print("Save");
}

void UUW_MainMenu_SaveDataSelect::Load(int32 InSlotIndex)
{
	//UNDONE 세이브로드와 연결하기
	CLog::Print(InSlotIndex);
	CLog::Print("Load");
}

void UUW_MainMenu_SaveDataSelect::Init(UUW_MainMenu_Confirm* NewConfirmWidget)
{
	ConfirmWidget = NewConfirmWidget;
	CheckTrue_Print(!ConfirmWidget, "ConfirmWidget is nullptr");	
}

void UUW_MainMenu_SaveDataSelect::Show(EMainMenuPhase NewType)
{
	SetFocus();

	Type = NewType;

	if (Type == EMainMenuPhase::Continue)
	{
		SlotIndex = 0;
		Load(SlotIndex);
		return;
	}
	else if (Type == EMainMenuPhase::NewGame)
	{
		//딱히 할건 없음
	}
	else if (Type == EMainMenuPhase::LoadGame)
	{
		//딱히 할건 없음	
	}
	else CheckTrue_Print(1, "Type is not Valid");

	{
		int32 prev = SlotIndex;
		SlotIndex = 0;
		SaveDataList->SetScrollOffset(SlotIndex * 1.078125);
		Glow(prev, SlotIndex);
	}

	SetVisibility(ESlateVisibility::Visible);
}

void UUW_MainMenu_SaveDataSelect::Hide()
{
	SetVisibility(ESlateVisibility::Collapsed);
}
