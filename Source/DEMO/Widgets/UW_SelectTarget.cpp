#include "Widgets/UW_SelectTarget.h"
#include "Global.h"

#include "Characters/TurnBasedCharacter.h"

void UUW_SelectTarget::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	// 위젯이 키 입력을 받을 수 있도록
	this->bIsFocusable = true;
}

void UUW_SelectTarget::NativeConstruct()
{
	Super::NativeConstruct();
}

FReply UUW_SelectTarget::NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent)
{
	if(!OnCanSelectTargetKeyDown.Execute())
		return Super::NativeOnKeyDown(InGeometry, InKeyEvent);

	const FKey Key = InKeyEvent.GetKey();

	if (Key == EKeys::W)
	{
		return FReply::Handled();
	}
	else if (Key == EKeys::A)
	{
		Left();
		return FReply::Handled();
	}
	else if (Key == EKeys::S)
	{
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

void UUW_SelectTarget::Left()
{
	TargetIndex--;
	TargetIndex = (TargetIndex + TargetArray.Num()) % TargetArray.Num();
	OwningActor->SetActorTransform(TargetArray[TargetIndex]->GetSelectTargetTransform());
	OnMoveDown.Broadcast(TargetArray[TargetIndex]);
}

void UUW_SelectTarget::Right()
{
	TargetIndex++;
	TargetIndex %= TargetArray.Num();
	OwningActor->SetActorTransform(TargetArray[TargetIndex]->GetSelectTargetTransform());
	OnMoveDown.Broadcast(TargetArray[TargetIndex]);
}

void UUW_SelectTarget::Confirm()
{
	OnConfirmDown.Broadcast(TargetArray[TargetIndex]);
}

void UUW_SelectTarget::SetOwningActor(AActor* NewOwningActor)
{
	OwningActor = NewOwningActor;
}

void UUW_SelectTarget::Activate(const TArray<ATurnBasedCharacter*>& InArray)
{
	TargetIndex = 0;
	TargetArray = InArray;
	this->SetKeyboardFocus();
	OnMoveDown.Broadcast(TargetArray[TargetIndex]);
}