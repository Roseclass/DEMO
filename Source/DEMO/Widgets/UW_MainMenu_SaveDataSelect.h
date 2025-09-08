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
class UEditableTextBox;
class UListView;
class UImage;
class UScaleBox;
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
	virtual FReply NativeOnKeyChar(const FGeometry& InGeometry, const FCharacterEvent& InCharEvent) override;
	virtual FReply NativeOnPreviewKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent) override;
	virtual FReply NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent) override;
public:

	//property
private:
	UUW_MainMenu_Confirm* ConfirmWidget;

	EMainMenuPhase PhaseType;

	int32 SlotIndex;
	int32 InputCursorLocation;
	TArray<FString> SaveSlotNames;
protected:
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
		UListView* SaveDataList;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
		UImage* Back_Background;

	//Input
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
		UScaleBox* Input_ScaleBox;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
		UImage* Input_Confirm;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
		UImage* Input_Cancel;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
		UEditableTextBox* Input_TextBox;

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
	void ChangeInputCursorLocation(int32 NewLocation);

	void Glow_List(int32 PrevIdx, int32 CurIdx);
	void Glow_Input();
	void Load(int32 InSlotIndex);

	void OnInput();
	void OffInput();
	UFUNCTION()void OnInputTextComitted(const FText& Text, ETextCommit::Type CommitMethod);
protected:
public:
	void Init(UUW_MainMenu_Confirm* NewConfirmWidget);
	void Show(EMainMenuPhase NewType);
	void Hide();
};

/*
* 메타데이터 값을 리스트 뷰에 추가
* 데이터 추가시에는 최대 오더를 구해서 +1 한 값으로 오더 설정
* 데이터 삭제시에는 그값만 제거
* 데이터 변동이있을때 위젯 갱신
*/


/*
*
* newgame시에 세이브 슬롯의 이름을 정하는 tpye추가
* 기본name은 slot+FString::FromInt(SaveDataList->getnum());
* type에서 enter시에 type에서 focus해제
* confirm <-> cancel에서 ad로 조종
* w입력시 tpye에 focus enter시 type가능하게
*
* 빈칸은 FString()기본형 IsEmpty로 체크
*
* Newgame Loadgame진입시에 order 적용 listview 오픈
* Load창에서 delete시에 당장은 order를 하지 않고 대기
* newgame창에서 order를 적용하고 save에서 init할때 order를 정돈하고 넣어주기때문에 걱정할필요 x
* 그래도 creategame에 order파라미터를 추가하고 배열에서 maxorder찾아서 +1 해서 넣어주자
*
* continue - getcurrentslot - load
* Newgame - order - open - selectslot - createnew - load
* loadgame - order - open - selectslot - load
*
*/