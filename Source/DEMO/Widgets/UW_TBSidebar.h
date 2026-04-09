#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GameplayTagContainer.h"
#include "UW_TBSidebar.generated.h"

class UVerticalBox;
class UWidgetAnimation;
class UImage;
class UUW_TBSidebar_Portrait;
class UDEMOCharacterUIDataRegistry;

UCLASS()
class DEMO_API UUW_TBSidebar : public UUserWidget
{
	GENERATED_BODY()
public:
protected:
public:

	//property
private:
protected:
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
		UVerticalBox* PortraitContainer;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
		TSubclassOf<UUW_TBSidebar_Portrait> PortraitClass;
public:

	//function
private:
protected:
public:
	void RemovePortrait(int32 Idx);
	void RemovePortraitsByTag(FGameplayTag DataTag);
	void InsertPortraitAt(int32 Idx, FGameplayTag DataTag);
	void AddPortrait(FGameplayTag DataTag);

	int32 GetCurrentSize() const;
};

DECLARE_DELEGATE_OneParam(FOnPortraitRemoveAnimEnd, UWidget*);

UCLASS()
class DEMO_API UUW_TBSidebar_Portrait : public UUserWidget
{
	GENERATED_BODY()
public:
protected:
public:
	virtual void NativeConstruct()override;
	virtual void NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent);
	virtual void NativeOnMouseLeave(const FPointerEvent& InMouseEvent);
	virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent);

	//property
private:
	FGameplayTag DataTag;
protected:
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
		UImage* Image;

	UPROPERTY(BlueprintReadOnly, Transient, meta = (BindWidgetAnim))
		UWidgetAnimation* Insert;

	UPROPERTY(BlueprintReadOnly, Transient, meta = (BindWidgetAnim))
		UWidgetAnimation* Remove;

	UPROPERTY(EditAnywhere)
		TObjectPtr<UDEMOCharacterUIDataRegistry> Registry;
public:
	FOnPortraitRemoveAnimEnd OnPortraitRemoveAnimEnd;

	//function
private:
	UFUNCTION()void RemoveAnimEndFunc();
protected:
public:
	void Init(FGameplayTag NewDataTag);
	void PlayInsertAnimation();
	void PlayRemoveAnimation();
	
	FORCEINLINE FGameplayTag GetDataTag()const { return DataTag; };
};
