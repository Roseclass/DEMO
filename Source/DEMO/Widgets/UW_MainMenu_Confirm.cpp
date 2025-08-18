#include "Widgets/UW_MainMenu_Confirm.h"
#include "Global.h"

#include "Components/Image.h"
#include "Components/TextBlock.h"

void UUW_MainMenu_Confirm::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	// 위젯이 키 입력을 받을 수 있도록
	this->bIsFocusable = true;
}

void UUW_MainMenu_Confirm::NativeConstruct()
{
	Super::NativeConstruct();
}

FReply UUW_MainMenu_Confirm::NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent)
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

void UUW_MainMenu_Confirm::Up()
{

}

void UUW_MainMenu_Confirm::Down()
{

}

void UUW_MainMenu_Confirm::Left()
{
	bIsConfirm = !bIsConfirm;
	Glow();
}

void UUW_MainMenu_Confirm::Right()
{
	bIsConfirm = !bIsConfirm;
	Glow();
}

void UUW_MainMenu_Confirm::Confirm()
{
	if (!bIsConfirm)
		CancelEvent();
	else
	{
		if (Type == EMainMenuPhase::NewGame)
		{
			ConfirmEvent();
			//딱히 할건 없음
		}
		else if (Type == EMainMenuPhase::LoadGame)
		{
			ConfirmEvent();
			//딱히 할건 없음
		}
		else CheckTrue_Print(1, "Type is not Valid");

	}
	CancelEvent.Reset();
	ConfirmEvent.Reset();
	Hide();
}

void UUW_MainMenu_Confirm::Glow()
{
	FSlateColor selected(FLinearColor(1.f, 1.f, 1.f, OpacityWhenSelected));
	FSlateColor notSelected(FLinearColor(1.f, 1.f, 1.f, OpacityWhenNotSelected));

	ConfirmImage->SetBrushTintColor(bIsConfirm ? selected : notSelected);
	CancelImage->SetBrushTintColor(!bIsConfirm ? selected : notSelected);
}

void UUW_MainMenu_Confirm::Init()
{
	//딱히 할건 없음
}

void UUW_MainMenu_Confirm::Show(EMainMenuPhase NewType, TFunction<void()> NewConfirmEvent, TFunction<void()> NewCancelEvent)
{
	SetFocus();

	Type = NewType;

	if (Type == EMainMenuPhase::Continue)
	{
		//딱히 할건 없음
	}
	else if (Type == EMainMenuPhase::NewGame)
	{
		PopupMessage->SetText(
			PopupMessageText[(int32)EMainMenuPhase::NewGame]
		/*FText::FromString(TEXT("이미 저장된 데이터가 있습니다.\n진행하시겠습니까?"))*/
		);
	}
	else if (Type == EMainMenuPhase::LoadGame)
	{
		//딱히 할건 없음
	}
	else CheckTrue_Print(1, "Type is not Valid");

	
	ConfirmEvent = NewConfirmEvent;
	CancelEvent = NewCancelEvent;

	bIsConfirm = 0;
	Glow();

	SetVisibility(ESlateVisibility::Visible);
}

void UUW_MainMenu_Confirm::Hide()
{
	SetVisibility(ESlateVisibility::Collapsed);
}
