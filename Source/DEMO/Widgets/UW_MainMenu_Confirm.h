#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Widgets/MainMenuTypes.h"
#include "UW_MainMenu_Confirm.generated.h"

/**
 * 메인 메뉴의 확인창
 * ex)변경된 설정을 저장하시겠습니까? Y/N
 * ex)이미 저장된 데이터가 있습니다 덮어 씌우시겠습니까? Y/N
 */

class UBorder;
class UImage;
class UTextBlock;

UCLASS()
class DEMO_API UUW_MainMenu_Confirm : public UUserWidget
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
	EMainMenuPhase Type;
	bool bIsConfirm;
	TFunction<void()> ConfirmEvent;
	TFunction<void()> CancelEvent;
protected:
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
		UImage* ConfirmImage;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
		UImage* CancelImage;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
		UTextBlock* PopupMessage;

	//Cover
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
		UBorder* FocusCatcher;

	//Datas
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Data")
		float OpacityWhenSelected = 1;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Data")
		float OpacityWhenNotSelected = 0.4;

	UPROPERTY(EditDefaultsOnly, Category = "Data", meta = (MultiLine = true))
		FText PopupMessageText[(int32)EMainMenuPhase::MAX];

public:

	//function
private:
	void Up();
	void Down();
	void Left();
	void Right();
	void Confirm();
	void Glow();
protected:
public:
	void Init();
	void Show(EMainMenuPhase NewType, TFunction<void()> NewConfirmEvent, TFunction<void()> NewCancelEvent);
	void Hide();
};

/*
* 하위 위젯들이나 메인 위젯이 숨겨져있다면 키다운 이벤트 핸들하지 않게 수정하기
* 리스트뷰의 크기를 제한하고 onkeydown 에 맞게 포커스를 활성화하고 ad를 통해 삭제할수있게하고 삭제시에 옆으로 빠지면서 한칸 밀리는 연출을 구현해보자
*/