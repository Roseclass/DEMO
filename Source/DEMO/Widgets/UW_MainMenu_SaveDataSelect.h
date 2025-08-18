#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Blueprint/IUserObjectListEntry.h"
#include "Widgets/MainMenuTypes.h"
#include "UW_MainMenu_SaveDataSelect.generated.h"

/**
 * SaveData를 선택해 정해진 함수를 실행 (load or save)
 */

class UBorder;
class UListView;
class UImage;
class UTextBlock;
class UUW_MainMenu_Confirm;

DECLARE_MULTICAST_DELEGATE(FSaveDataSelectDelegate);

UCLASS(BlueprintType)
class DEMO_API UMainMenu_SaveDataSelect_ListData : public UObject
{
	GENERATED_BODY()

public:
	// 표시할 텍스트
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ListEntry")
		FText SaveDataText;
};

UCLASS()
class DEMO_API UUW_MainMenu_SaveDataSelect_ListEntry : public UUserWidget, public IUserObjectListEntry
{
	GENERATED_BODY()
public:
protected:
	virtual void NativeOnListItemObjectSet(UObject* ListItemObject) override;
	virtual void NativeOnItemSelectionChanged(bool bIsSelected) override;
public:

	//property
private:
protected:
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget, MultiLine = true))
		UTextBlock* SaveDataText;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget, MultiLine = true))
		UImage* BackgroundImage;

	//Datas
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Data")
		float OpacityWhenSelected = 1;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Data")
		float OpacityWhenNotSelected = 0.4;

public:

	//function
private:
	void ApplySelectionGlow();
protected:
public:
};

UCLASS()
class DEMO_API UUW_MainMenu_SaveDataSelect : public UUserWidget
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
	UUW_MainMenu_Confirm* ConfirmWidget;

	EMainMenuPhase Type;
	int32 SlotIndex;
protected:
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
		UListView* SaveDataList;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
		UImage* Back_Background;

	//Cover
	UPROPERTY(BlueprintReadOnly, Category = "MainMenu", meta = (BindWidget))
		UBorder* FocusCatcher;

	//Datas
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Data")
		float OpacityWhenSelected = 1;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Data")
		float OpacityWhenNotSelected = 0.4;
public:
	FSaveDataSelectDelegate OnBackConfirmed;

	//function
private:
	void Up();
	void Down();
	void Left();
	void Right();
	void Confirm();

	void Glow(int32 PrevIdx, int32 CurIdx);
	void Save(int32 InSlotIndex);
	void Load(int32 InSlotIndex);
protected:
public:
	void Init(UUW_MainMenu_Confirm* NewConfirmWidget);
	void Show(EMainMenuPhase NewType);
	void Hide();
};
