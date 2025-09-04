#include "Widgets/UW_MainMenu_SaveDataSelect.h"
#include "Global.h"

#include "Components/EditableTextBox.h"
#include "Components/ListView.h"
#include "Components/Image.h"
#include "Components/ScaleBox.h"
#include "Components/ScrollBar.h"
#include "Components/TextBlock.h"

#include "DEMOPlayerState.h"
#include "SaveLoadSubsystem.h"

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

#define CURSOR_INPUT 0
#define CURSOR_CONFIRM 1
#define CURSOR_CANCEL 2

void UUW_MainMenu_SaveDataSelect::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	// 위젯이 키 입력을 받을 수 있도록
	this->bIsFocusable = true;

	// saveslotname commit이 끝나면 wasd모드로 전환
	Input_TextBox->OnTextCommitted.AddDynamic(this, &UUW_MainMenu_SaveDataSelect::OnInputTextComitted);
}

void UUW_MainMenu_SaveDataSelect::NativeConstruct()
{
	Super::NativeConstruct();

	ADEMOPlayerState* PS = Cast<ADEMOPlayerState>(UGameplayStatics::GetPlayerState(GetWorld(), 0));

	TArray<FSaveMetaData> metaDatas = PS->GetAllSaveMetaData();

	for (const auto& i : metaDatas)
	{
		SaveSlotNames.Add(i.SlotName);

		UMainMenu_SaveDataSelect_ListData* obj = NewObject<UMainMenu_SaveDataSelect_ListData>();
		obj->SaveDataText = !i.bIsEmpty ?
			FText::FromString(FString::Printf(TEXT("%s\n%s"), *i.SlotName, *i.Date.ToString())) :
			FText::FromString("Empty");
		SaveDataList->AddItem(obj);
	}
}

FReply UUW_MainMenu_SaveDataSelect::NativeOnKeyChar(const FGeometry& InGeometry, const FCharacterEvent& InCharEvent)
{
	const TCHAR Key = InCharEvent.GetCharacter();

	// ignore first input to TextBox
	if (Key == 'W' || Key == 'w')
	{
		Up();
		return FReply::Handled();
	}
	else if (Key == ' ')
	{
		Confirm();
		return FReply::Handled();
	}

	return Super::NativeOnKeyChar(InGeometry, InCharEvent);
}

FReply UUW_MainMenu_SaveDataSelect::NativeOnPreviewKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent)
{
	return Super::NativeOnPreviewKeyDown(InGeometry, InKeyEvent);
}

FReply UUW_MainMenu_SaveDataSelect::NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent)
{
	const FKey Key = InKeyEvent.GetKey();

	if (Key == EKeys::A)
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

	// 위젯에서 처리하지 않은 키는 부모에게 전달
	return Super::NativeOnKeyDown(InGeometry, InKeyEvent);
}

void UUW_MainMenu_SaveDataSelect::Up()
{
	if (Input_ScaleBox->IsVisible())
	{
		ChangeInputCursorLocation(CURSOR_INPUT);
		Input_TextBox->SetKeyboardFocus();
		return;
	}

	int32 prev = SlotIndex;
	SlotIndex--;

	if (SlotIndex < 0)SlotIndex = SaveDataList->GetNumItems();

	Glow_List(prev, SlotIndex);
}

void UUW_MainMenu_SaveDataSelect::Down()
{
	CheckTrue(Input_ScaleBox->IsVisible());

	int32 prev = SlotIndex;
	SlotIndex++;

	if (SaveDataList->GetNumItems() < SlotIndex)SlotIndex = 0;

	Glow_List(prev,SlotIndex);
}

void UUW_MainMenu_SaveDataSelect::Left()
{
	//Input_ScaleBox 전용
	CheckTrue(!Input_ScaleBox->IsVisible());
	ChangeInputCursorLocation(CURSOR_CONFIRM);
}

void UUW_MainMenu_SaveDataSelect::Right()
{
	//Input_ScaleBox 전용
	CheckTrue(!Input_ScaleBox->IsVisible());
	ChangeInputCursorLocation(CURSOR_CANCEL);
}

void UUW_MainMenu_SaveDataSelect::Confirm()
{
	CheckTrue_Print(!ConfirmWidget, "ConfirmWidget is nullptr");
	
	//BACK
	if (SlotIndex == SaveDataList->GetNumItems())
	{
		OnBackConfirmed.Broadcast();
		return;
	}

	ADEMOPlayerState* PS = Cast<ADEMOPlayerState>(UGameplayStatics::GetPlayerState(GetWorld(), 0));
	USaveLoadSubsystem* SS = GetGameInstance()->GetSubsystem<USaveLoadSubsystem>();

	// ELSE
	if (PhaseType == EMainMenuPhase::NewGame)
	{
		//INPUT
		if (Input_ScaleBox->IsVisible())
		{
			if (InputCursorLocation == CURSOR_CONFIRM)
			{
				CheckTrue_Print(!SaveSlotNames.IsValidIndex(SlotIndex), "SlotIndex is out of range");
				SS->DeleteData(SlotIndex);
				SS->CreateNewData(SlotIndex, SaveSlotNames[SlotIndex] = Input_TextBox->GetText().ToString());				
				SS->LoadData(SlotIndex);
				OffInput();
			}
			else if (InputCursorLocation == CURSOR_CANCEL)
			{
				OffInput();
			}
			else
			{
				CheckTrue_Print(1, "InputCursorLocation is out of range");
			}
		}
		else if (PS->IsEmpty(SlotIndex))
		{
			this->OnInput();
		}
		else
		{
			ConfirmWidget->Show(
				PhaseType,
				[=]()// confirm event
				{
					this->OnInput();
				},
				[=]()// hide event
				{
					this->SetFocus();
				});
		}
	}
	else if (PhaseType == EMainMenuPhase::LoadGame)
	{
		SS->LoadData(SlotIndex);
	}
	else CheckTrue_Print(1, "Type is not Valid");
}

void UUW_MainMenu_SaveDataSelect::ChangeInputCursorLocation(int32 NewLocation)
{
	CheckTrue_Print(CURSOR_CANCEL < NewLocation, "CURSOR_CANCEL < NewLocation");
	CheckTrue_Print(NewLocation < CURSOR_INPUT, "NewLocation < CURSOR_INPUT");
	InputCursorLocation = NewLocation;
	Glow_Input();
}

void UUW_MainMenu_SaveDataSelect::Glow_List(int32 PrevIdx, int32 CurIdx)
{
	FSlateColor selected(FLinearColor(1.f, 1.f, 1.f, OpacityWhenSelected));
	FSlateColor notSelected(FLinearColor(1.f, 1.f, 1.f, OpacityWhenNotSelected));	

	Back_Background->SetBrushTintColor(SlotIndex == SaveDataList->GetNumItems() ? selected : notSelected);

	if (SaveDataList->GetListItems().IsValidIndex(PrevIdx))
		SaveDataList->SetItemSelection(SaveDataList->GetItemAt(PrevIdx), 0);

	if (SaveDataList->GetListItems().IsValidIndex(CurIdx))
		SaveDataList->SetItemSelection(SaveDataList->GetItemAt(CurIdx), 1);

	GetWorld()->GetTimerManager().SetTimerForNextTick([&]()
		{
			if (SaveDataList->GetListItems().IsValidIndex(SlotIndex))SaveDataList->SetScrollOffset(SlotIndex * 1.078125);
		});
}

void UUW_MainMenu_SaveDataSelect::Glow_Input()
{
	CheckTrue_Print(CURSOR_CANCEL < InputCursorLocation, "CURSOR_CANCEL < InputCursorLocation");
	CheckTrue_Print(InputCursorLocation < CURSOR_INPUT, "InputCursorLocation < CURSOR_INPUT");

	FSlateColor selected(FLinearColor(1.f, 1.f, 1.f, OpacityWhenSelected));
	FSlateColor notSelected(FLinearColor(1.f, 1.f, 1.f, OpacityWhenNotSelected));

	Input_Confirm->SetBrushTintColor(InputCursorLocation & CURSOR_CONFIRM ? selected : notSelected);
	Input_Cancel->SetBrushTintColor(InputCursorLocation & CURSOR_CANCEL ? selected : notSelected);
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

void UUW_MainMenu_SaveDataSelect::OnInput()
{
	ChangeInputCursorLocation(CURSOR_INPUT);
	Input_TextBox->SetKeyboardFocus();
	Input_TextBox->SetText(FText::FromString("Slot" + FString::FromInt(SlotIndex)));
	Input_ScaleBox->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
}

void UUW_MainMenu_SaveDataSelect::OffInput()
{
	Input_ScaleBox->SetVisibility(ESlateVisibility::Collapsed);
}

void UUW_MainMenu_SaveDataSelect::OnInputTextComitted(const FText& Text, ETextCommit::Type CommitMethod)
{
	switch (CommitMethod)
	{
	case ETextCommit::Default:
	{
		break; 
	}
	case ETextCommit::OnEnter:
	{
		GetWorld()->GetTimerManager().SetTimerForNextTick([&]()
			{
				ChangeInputCursorLocation(CURSOR_CONFIRM);
				this->SetFocus();
			});
		break;
	}
	case ETextCommit::OnUserMovedFocus:
	{
		if (HasAnyUserFocus())return;
		GetWorld()->GetTimerManager().SetTimerForNextTick([&]()
			{
				ChangeInputCursorLocation(CURSOR_CANCEL);
				this->SetFocus();
			});
		break;
	}
	case ETextCommit::OnCleared:
	{
		if (Input_TextBox->HasKeyboardFocus() || !HasAnyUserFocus())
		{
			GetWorld()->GetTimerManager().SetTimerForNextTick([&]()
				{
					this->SetFocus();
				});
		}
		break;
	}
	default:
		break;
	}
}

void UUW_MainMenu_SaveDataSelect::Init(UUW_MainMenu_Confirm* NewConfirmWidget)
{
	ConfirmWidget = NewConfirmWidget;
	CheckTrue_Print(!ConfirmWidget, "ConfirmWidget is nullptr");	
}

void UUW_MainMenu_SaveDataSelect::Show(EMainMenuPhase NewType)
{
	SetFocus();

	PhaseType = NewType;

	if (PhaseType == EMainMenuPhase::Continue)
	{
		SlotIndex = 0;
		Load(SlotIndex);
		return;
	}
	else if (PhaseType == EMainMenuPhase::NewGame)
	{
		//딱히 할건 없음
	}
	else if (PhaseType == EMainMenuPhase::LoadGame)
	{
		//딱히 할건 없음	
	}
	else CheckTrue_Print(1, "Type is not Valid");

	{
		int32 prev = SlotIndex;
		SlotIndex = 0;
		SaveDataList->SetScrollOffset(SlotIndex * 1.078125);
		Glow_List(prev, SlotIndex);
	}

	OffInput();

	SetVisibility(ESlateVisibility::Visible);
}

void UUW_MainMenu_SaveDataSelect::Hide()
{
	SetVisibility(ESlateVisibility::Collapsed);
}
