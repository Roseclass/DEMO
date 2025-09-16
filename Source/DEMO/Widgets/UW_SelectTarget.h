#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UW_SelectTarget.generated.h"

/**
 * wasd + space로 조종
 * 현재 어떤 캐릭터를 선택했는지 보여주고 선택해서 정해진 함수를 broadcast
 */

class ATurnBasedCharacter;

DECLARE_MULTICAST_DELEGATE_OneParam(FOnSelectTargetKeyDown, ATurnBasedCharacter*);
DECLARE_DELEGATE_RetVal(bool, FOnCanSelectTargetKeyDown);

UCLASS()
class DEMO_API UUW_SelectTarget : public UUserWidget
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
	bool bActive;
	AActor* OwningActor;

	int32 TargetIndex;
	TArray<ATurnBasedCharacter*> TargetArray;
protected:
public:
	FOnSelectTargetKeyDown OnMoveDown;
	FOnSelectTargetKeyDown OnConfirmDown;
	FOnCanSelectTargetKeyDown OnCanSelectTargetKeyDown;

	//function
private:
	void Left();
	void Right();
	void Confirm();
protected:
public:
	void SetOwningActor(AActor* NewOwningActor);
	void Activate(const TArray<ATurnBasedCharacter*>& InArray);
};

/*
* 캐릭터의 신컴포넌트에 위치하면됨
*/