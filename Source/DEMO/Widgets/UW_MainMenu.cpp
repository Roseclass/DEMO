#include "Widgets/UW_MainMenu.h"
#include "Global.h"

#include "Components/Border.h"
#include "Components/Button.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"

#include "SaveLoad/SaveManager.h"

#include "Widgets/UW_MainMenu_SaveDataSelect.h"
#include "Widgets/UW_MainMenu_Settings.h"
#include "Widgets/UW_MainMenu_Confirm.h"

void UUW_MainMenu::NativeOnInitialized()
{
	Super::NativeOnInitialized();


	SaveDataSelect->Init(ConfirmWidget);
	SaveDataSelect->OnBackConfirmed.AddLambda([=]()
		{
			SaveDataSelect->Hide();
			this->ShowMainMenu();
		});

	//Settings->Init(Confirm);

	ConfirmWidget->Init();

	// 위젯이 키 입력을 받을 수 있도록
	this->bIsFocusable = true;
}

void UUW_MainMenu::NativeConstruct()
{
	Super::NativeConstruct();

	ActivatedPhaseMap.Add(EMainMenuPhase::MIN, 1);
	ActivatedPhaseMap.Add(EMainMenuPhase::Continue, 1);
	ActivatedPhaseMap.Add(EMainMenuPhase::NewGame, 1);
	ActivatedPhaseMap.Add(EMainMenuPhase::LoadGame, 1);
	ActivatedPhaseMap.Add(EMainMenuPhase::Settings, 1);
	ActivatedPhaseMap.Add(EMainMenuPhase::ExitGame, 1);
	ActivatedPhaseMap.Add(EMainMenuPhase::MAX, 1);

	if (USaveManager::GetCurrentSaveSlot().IsEmpty())
	{
		Location = EMainMenuPhase::NewGame;
		ActivatedPhaseMap[EMainMenuPhase::Continue] = 0;
		Continue_Text->SetColorAndOpacity(TextColorWhenNotActivated);
	}

	Glow();

	SetFocus();
}

FReply UUW_MainMenu::NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent)
{

	if(bIsHide)	return FReply::Unhandled();

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

	return Super::NativeOnKeyDown(InGeometry, InKeyEvent);
}

void UUW_MainMenu::Up()
{
	CheckTrue(Location == EMainMenuPhase::Continue);
	CheckTrue_Print(Location == EMainMenuPhase::MIN, "Location is MIN");
	CheckTrue_Print(Location == EMainMenuPhase::MAX, "Location is MAX");

	EMainMenuPhase temp = Location;

	while (temp != EMainMenuPhase::MIN)
	{
		switch (temp)
		{
		case EMainMenuPhase::Continue:temp = EMainMenuPhase::MIN;
			break;
		case EMainMenuPhase::NewGame:temp = EMainMenuPhase::Continue;
			break;
		case EMainMenuPhase::LoadGame:temp = EMainMenuPhase::NewGame;
			break;
		case EMainMenuPhase::Settings:temp = EMainMenuPhase::LoadGame;
			break;
		case EMainMenuPhase::ExitGame:temp = EMainMenuPhase::Settings;
			break;
		default:break;
		}

		if (ActivatedPhaseMap[temp])break;
	}

	CheckTrue(temp == EMainMenuPhase::MIN);

	Location = temp;

	Glow();
}

void UUW_MainMenu::Down()
{
	CheckTrue(Location == EMainMenuPhase::ExitGame);
	CheckTrue_Print(Location == EMainMenuPhase::MIN, "Location is MIN");
	CheckTrue_Print(Location == EMainMenuPhase::MAX, "Location is MAX");

	EMainMenuPhase temp = Location;

	while (temp != EMainMenuPhase::MAX)
	{
		switch (temp)
		{
		case EMainMenuPhase::Continue:temp = EMainMenuPhase::NewGame;
			break;
		case EMainMenuPhase::NewGame:temp = EMainMenuPhase::LoadGame;
			break;
		case EMainMenuPhase::LoadGame:temp = EMainMenuPhase::Settings;
			break;
		case EMainMenuPhase::Settings:temp = EMainMenuPhase::ExitGame;
			break;
		case EMainMenuPhase::ExitGame:temp = EMainMenuPhase::MAX;
			break;
		default:break;
		}

		if (ActivatedPhaseMap[temp])break;
	}

	CheckTrue(temp == EMainMenuPhase::MAX);

	Location = temp;

	Glow();
}

void UUW_MainMenu::Left()
{
	PrintLine()
}

void UUW_MainMenu::Right()
{
	PrintLine()
}

void UUW_MainMenu::Confirm()
{
	switch (Location)
	{
	case EMainMenuPhase::Continue:ContinueConfirmed();
		break;
	case EMainMenuPhase::NewGame:NewGameConfirmed();
		break;
	case EMainMenuPhase::LoadGame:LoadGameConfirmed();
		break;
	case EMainMenuPhase::Settings:SettingsConfirmed();
		break;
	case EMainMenuPhase::ExitGame:ExitGameConfirmed();
		break;
	case EMainMenuPhase::MAX:
	{
		CLog::Print(FString::Printf(TEXT("%s %s"), *FString(__FUNCTION__), *CHelpers::EnumToString(FString("EMainMenuPhase"), EMainMenuPhase::MAX)));
		return;
		break;
	}
	default:
		break;
	}
}

void UUW_MainMenu::Glow()
{
	FSlateColor selected(FLinearColor(1.f, 1.f, 1.f, OpacityWhenSelected));
	FSlateColor notSelected(FLinearColor(1.f, 1.f, 1.f, OpacityWhenNotSelected));

	Continue_Background->SetBrushTintColor(Location == EMainMenuPhase::Continue ? selected : notSelected);
	NewGame_Background->SetBrushTintColor(Location == EMainMenuPhase::NewGame ? selected : notSelected);
	LoadGame_Background->SetBrushTintColor(Location == EMainMenuPhase::LoadGame ? selected : notSelected);
	Settings_Background->SetBrushTintColor(Location == EMainMenuPhase::Settings ? selected : notSelected);
	ExitGame_Background->SetBrushTintColor(Location == EMainMenuPhase::ExitGame ? selected : notSelected);
}

void UUW_MainMenu::ShowMainMenu()
{
	bIsHide = 0;

	SetFocus();

	// check Continue
	if (USaveManager::GetCurrentSaveSlot().IsEmpty())
	{
		Location = EMainMenuPhase::NewGame;
		ActivatedPhaseMap[EMainMenuPhase::Continue] = 0;
		Continue_Text->SetColorAndOpacity(
			ActivatedPhaseMap[EMainMenuPhase::Continue] ? TextColorWhenActivated : TextColorWhenNotActivated);
	}

	Continue_Background->SetVisibility(ESlateVisibility::Visible);
	Continue_Frame->SetVisibility(ESlateVisibility::Visible);
	Continue_Text->SetVisibility(ESlateVisibility::Visible);
	Continue_Text->SetColorAndOpacity(
		ActivatedPhaseMap[EMainMenuPhase::Continue] ? TextColorWhenActivated : TextColorWhenNotActivated);

	NewGame_Background->SetVisibility(ESlateVisibility::Visible);
	NewGame_Frame->SetVisibility(ESlateVisibility::Visible);
	NewGame_Text->SetVisibility(ESlateVisibility::Visible);
	NewGame_Text->SetColorAndOpacity(
		ActivatedPhaseMap[EMainMenuPhase::NewGame] ? TextColorWhenActivated : TextColorWhenNotActivated);

	LoadGame_Background->SetVisibility(ESlateVisibility::Visible);
	LoadGame_Frame->SetVisibility(ESlateVisibility::Visible);
	LoadGame_Text->SetVisibility(ESlateVisibility::Visible);
	LoadGame_Text->SetColorAndOpacity(
		ActivatedPhaseMap[EMainMenuPhase::LoadGame] ? TextColorWhenActivated : TextColorWhenNotActivated);

	Settings_Background->SetVisibility(ESlateVisibility::Visible);
	Settings_Frame->SetVisibility(ESlateVisibility::Visible);
	Settings_Text->SetVisibility(ESlateVisibility::Visible);
	Settings_Text->SetColorAndOpacity(
		ActivatedPhaseMap[EMainMenuPhase::Settings] ? TextColorWhenActivated : TextColorWhenNotActivated);

	ExitGame_Background->SetVisibility(ESlateVisibility::Visible);
	ExitGame_Frame->SetVisibility(ESlateVisibility::Visible);
	ExitGame_Text->SetVisibility(ESlateVisibility::Visible);
	ExitGame_Text->SetColorAndOpacity(
		ActivatedPhaseMap[EMainMenuPhase::ExitGame] ? TextColorWhenActivated : TextColorWhenNotActivated);

	FocusCatcher->SetVisibility(ESlateVisibility::Visible);

	Glow();
}

void UUW_MainMenu::HideMainMenu()
{
	bIsHide = 1;
	
	Continue_Background->SetVisibility(ESlateVisibility::Collapsed);
	Continue_Frame->SetVisibility(ESlateVisibility::Collapsed);
	Continue_Text->SetVisibility(ESlateVisibility::Collapsed);

	NewGame_Background->SetVisibility(ESlateVisibility::Collapsed);
	NewGame_Frame->SetVisibility(ESlateVisibility::Collapsed);
	NewGame_Text->SetVisibility(ESlateVisibility::Collapsed);

	LoadGame_Background->SetVisibility(ESlateVisibility::Collapsed);
	LoadGame_Frame->SetVisibility(ESlateVisibility::Collapsed);
	LoadGame_Text->SetVisibility(ESlateVisibility::Collapsed);

	Settings_Background->SetVisibility(ESlateVisibility::Collapsed);
	Settings_Frame->SetVisibility(ESlateVisibility::Collapsed);
	Settings_Text->SetVisibility(ESlateVisibility::Collapsed);

	ExitGame_Background->SetVisibility(ESlateVisibility::Collapsed);
	ExitGame_Frame->SetVisibility(ESlateVisibility::Collapsed);
	ExitGame_Text->SetVisibility(ESlateVisibility::Collapsed);

	FocusCatcher->SetVisibility(ESlateVisibility::Collapsed);
}

void UUW_MainMenu::ContinueConfirmed()
{
	HideMainMenu();
	SaveDataSelect->Show(EMainMenuPhase::Continue);
}

void UUW_MainMenu::NewGameConfirmed()
{
	HideMainMenu();
	SaveDataSelect->Show(EMainMenuPhase::NewGame);
}

void UUW_MainMenu::LoadGameConfirmed()
{
	HideMainMenu();
	SaveDataSelect->Show(EMainMenuPhase::LoadGame);
}

void UUW_MainMenu::SettingsConfirmed()
{
	HideMainMenu();
	//Show();
}

void UUW_MainMenu::ExitGameConfirmed()
{
	UKismetSystemLibrary::QuitGame(GetWorld(), GetOwningPlayer(), EQuitPreference::Quit, 0);
}
