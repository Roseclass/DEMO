#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/Image.h"    // ← 꼭 포함
#include "Components/Button.h"   // ← 꼭 포함
#include "Widgets/MainMenuTypes.h"
#include "UW_MainMenu.generated.h"

/**
 * MainMenuPhase의 HUD역할
 */

class UImage;
class UButton;
class UBorder;
class UTextBlock;
class UUW_MainMenu_SaveDataSelect;
class UUW_MainMenu_Settings;
class UUW_MainMenu_Confirm;

UCLASS()
class DEMO_API UUW_MainMenu : public UUserWidget
{
	GENERATED_BODY()
public:
protected:
	virtual void NativeOnInitialized() override;
	virtual void NativeConstruct() override;
	virtual FReply NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent) override;
public:

	//property
private:
	bool bIsHide;
	EMainMenuPhase Location = EMainMenuPhase::Continue;
	TMap<EMainMenuPhase, bool> ActivatedPhaseMap;
protected:
	//Continue
	UPROPERTY(BlueprintReadOnly, Category = "MainMenu", meta = (BindWidget))
		UImage* Continue_Background;

	UPROPERTY(BlueprintReadOnly, Category = "MainMenu", meta = (BindWidget))
		UImage* Continue_Frame;

	UPROPERTY(BlueprintReadOnly, Category = "MainMenu", meta = (BindWidget))
		UTextBlock* Continue_Text;


	//NewGame
	UPROPERTY(BlueprintReadOnly, Category = "MainMenu", meta = (BindWidget))
		UImage* NewGame_Background;

	UPROPERTY(BlueprintReadOnly, Category = "MainMenu", meta = (BindWidget))
		UImage* NewGame_Frame;

	UPROPERTY(BlueprintReadOnly, Category = "MainMenu", meta = (BindWidget))
		UTextBlock* NewGame_Text;

	//LoadGame
	UPROPERTY(BlueprintReadOnly, Category = "MainMenu", meta = (BindWidget))
		UImage* LoadGame_Background;

	UPROPERTY(BlueprintReadOnly, Category = "MainMenu", meta = (BindWidget))
		UImage* LoadGame_Frame;

	UPROPERTY(BlueprintReadOnly, Category = "MainMenu", meta = (BindWidget))
		UTextBlock* LoadGame_Text;

	//Settings
	UPROPERTY(BlueprintReadOnly, Category = "MainMenu", meta = (BindWidget))
		UImage* Settings_Background;

	UPROPERTY(BlueprintReadOnly, Category = "MainMenu", meta = (BindWidget))
		UImage* Settings_Frame;

	UPROPERTY(BlueprintReadOnly, Category = "MainMenu", meta = (BindWidget))
		UTextBlock* Settings_Text;

	//ExitGame
	UPROPERTY(BlueprintReadOnly, Category = "MainMenu", meta = (BindWidget))
		UImage* ExitGame_Background;

	UPROPERTY(BlueprintReadOnly, Category = "MainMenu", meta = (BindWidget))
		UImage* ExitGame_Frame;

	UPROPERTY(BlueprintReadOnly, Category = "MainMenu", meta = (BindWidget))
		UTextBlock* ExitGame_Text;

	//Cover
	UPROPERTY(BlueprintReadOnly, Category = "MainMenu", meta = (BindWidget))
		UBorder* FocusCatcher;

	//Datas
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Data")
		float OpacityWhenSelected = 1;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Data")
		float OpacityWhenNotSelected = 0.4;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Data")
		FSlateColor TextColorWhenActivated = FSlateColor(FLinearColor(1, 1, 1, 1));

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Data")
		FSlateColor TextColorWhenNotActivated = FSlateColor(FLinearColor(0.2, 0.2, 0.2, 1));


	//Children
	UPROPERTY(BlueprintReadOnly, Category = "SaveDataSelect", meta = (BindWidget))
		UUW_MainMenu_SaveDataSelect* SaveDataSelect;

	UPROPERTY(BlueprintReadOnly, Category = "Settings", meta = (BindWidget))
		UUW_MainMenu_Settings* Settings;

	UPROPERTY(BlueprintReadOnly, Category = "Confirm", meta = (BindWidget))
		UUW_MainMenu_Confirm* ConfirmWidget;

public:

	//function
private:
	void Up();
	void Down();
	void Left();
	void Right();
	void Confirm();
	void Glow();

	void ShowMainMenu();
	void HideMainMenu();

	void ContinueConfirmed();
	void NewGameConfirmed();
	void LoadGameConfirmed();
	void SettingsConfirmed();
	void ExitGameConfirmed();
protected:
public:
};
