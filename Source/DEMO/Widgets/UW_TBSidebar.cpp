#include "Widgets/UW_TBSidebar.h"
#include "Global.h"

#include "Animation/WidgetAnimation.h"

#include "Components/Image.h"
#include "Components/VerticalBox.h"
#include "Components/VerticalBoxSlot.h"

#include "Datas/UITypes.h"

void UUW_TBSidebar::RemovePortrait(int32 Idx)
{
	UUW_TBSidebar_Portrait* portrait = Cast<UUW_TBSidebar_Portrait>(PortraitContainer->GetChildAt(Idx));
	CheckTrue(!portrait);

	portrait->OnPortraitRemoveAnimEnd.BindLambda(
		[this](UWidget* Portrait)
		{
			PortraitContainer->RemoveChild(Portrait);
		}
	);
	portrait->PlayRemoveAnimation();
}

void UUW_TBSidebar::RemovePortraitsByTag(FGameplayTag DataTag)
{
	TArray<UWidget*> widgets = PortraitContainer->GetAllChildren();
	
	for (auto widget : widgets)
	{
		UUW_TBSidebar_Portrait* portrait = Cast<UUW_TBSidebar_Portrait>(widget);
		if (!portrait)continue;
		if (portrait->GetDataTag() != DataTag)continue;
		portrait->OnPortraitRemoveAnimEnd.BindLambda(
			[this](UWidget* Portrait)
			{
				PortraitContainer->RemoveChild(Portrait);
			}
		);
		portrait->PlayRemoveAnimation();
	}
}

void UUW_TBSidebar::InsertPortraitAt(int32 Idx, FGameplayTag DataTag)
{
	UUW_TBSidebar_Portrait* child = CreateWidget<UUW_TBSidebar_Portrait>(GetOwningPlayer(), PortraitClass);
	child->Init(DataTag);

	PortraitContainer->InsertChildAt(Idx, child);
	child->PlayInsertAnimation();
}

void UUW_TBSidebar::AddPortrait(FGameplayTag DataTag)
{
	UUW_TBSidebar_Portrait* child = CreateWidget<UUW_TBSidebar_Portrait>(GetOwningPlayer(), PortraitClass);
	child->Init(DataTag);

	PortraitContainer->AddChildToVerticalBox(child);
	child->PlayInsertAnimation();
}

int32 UUW_TBSidebar::GetCurrentSize() const
{
	return PortraitContainer->GetChildrenCount();
}

/////////////////////////////////////////////////////////////////////////////////////////////
////////////////////UUW_TBSidebar_Portrait///////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////

void UUW_TBSidebar_Portrait::NativeConstruct()
{
	Super::NativeConstruct();

	FWidgetAnimationDynamicEvent func;
	func.BindUFunction(this, "RemoveAnimEndFunc");
	BindToAnimationFinished(Remove, func);

	CheckTrue_Print(!Registry, " Registry is nullptr");
}

void UUW_TBSidebar_Portrait::NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	Super::NativeOnMouseEnter(InGeometry, InMouseEvent);

}

void UUW_TBSidebar_Portrait::NativeOnMouseLeave(const FPointerEvent& InMouseEvent)
{
	Super::NativeOnMouseLeave(InMouseEvent);
}

FReply UUW_TBSidebar_Portrait::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	return Super::NativeOnMouseButtonDown(InGeometry, InMouseEvent);

}

void UUW_TBSidebar_Portrait::RemoveAnimEndFunc()
{
	OnPortraitRemoveAnimEnd.ExecuteIfBound(this);
}

void UUW_TBSidebar_Portrait::Init(FGameplayTag NewDataTag)
{
	DataTag = NewDataTag;

	if (!Registry->CharacterUIDataMap.Contains(NewDataTag))
	{
		CLog::Print("UUW_TBSidebar_Portrait::Init Registry NewDataTag not contains!!");
		return;
	}

	Image->SetBrushResourceObject(Registry->CharacterUIDataMap[NewDataTag].PortraitIconResourceObject);
}

void UUW_TBSidebar_Portrait::PlayInsertAnimation()
{
	PlayAnimationForward(Insert);
}

void UUW_TBSidebar_Portrait::PlayRemoveAnimation()
{
	PlayAnimationForward(Remove);
}
