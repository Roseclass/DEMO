#include "Widgets/UW_MainMenu.h"
#include "Global.h"

#include "Components/Border.h"
#include "Components/Button.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"

#include "Widgets/UW_MainMenu_SaveDataSelect.h"
#include "Widgets/UW_MainMenu_Settings.h"
#include "Widgets/UW_MainMenu_Confirm.h"

void UUW_MainMenu::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	//if(!SAVEDATA[0])
	//	Continue_Button->SetIsEnabled(0);

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
	switch (Location)
	{
	case EMainMenuPhase::Continue:return;
		break;
	case EMainMenuPhase::NewGame:Location = EMainMenuPhase::Continue;
		break;
	case EMainMenuPhase::LoadGame:Location = EMainMenuPhase::NewGame;
		break;
	case EMainMenuPhase::Settings:Location = EMainMenuPhase::LoadGame;
		break;
	case EMainMenuPhase::ExitGame:Location = EMainMenuPhase::Settings;
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
	
	Glow();
}

void UUW_MainMenu::Down()
{
	switch (Location)
	{
	case EMainMenuPhase::Continue:Location = EMainMenuPhase::NewGame;
		break;
	case EMainMenuPhase::NewGame:Location = EMainMenuPhase::LoadGame;
		break;
	case EMainMenuPhase::LoadGame:Location = EMainMenuPhase::Settings;
		break;
	case EMainMenuPhase::Settings:Location = EMainMenuPhase::ExitGame;
		break;
	case EMainMenuPhase::ExitGame:return;
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

	Continue_Background->SetVisibility(ESlateVisibility::Visible);
	Continue_Frame->SetVisibility(ESlateVisibility::Visible);
	Continue_Text->SetVisibility(ESlateVisibility::Visible);

	NewGame_Background->SetVisibility(ESlateVisibility::Visible);
	NewGame_Frame->SetVisibility(ESlateVisibility::Visible);
	NewGame_Text->SetVisibility(ESlateVisibility::Visible);

	LoadGame_Background->SetVisibility(ESlateVisibility::Visible);
	LoadGame_Frame->SetVisibility(ESlateVisibility::Visible);
	LoadGame_Text->SetVisibility(ESlateVisibility::Visible);

	Settings_Background->SetVisibility(ESlateVisibility::Visible);
	Settings_Frame->SetVisibility(ESlateVisibility::Visible);
	Settings_Text->SetVisibility(ESlateVisibility::Visible);

	ExitGame_Background->SetVisibility(ESlateVisibility::Visible);
	ExitGame_Frame->SetVisibility(ESlateVisibility::Visible);
	ExitGame_Text->SetVisibility(ESlateVisibility::Visible);

	FocusCatcher->SetVisibility(ESlateVisibility::Visible);
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
