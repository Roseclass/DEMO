#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GameplayTagContainer.h"
#include "UW_TBSelect.generated.h"

/**
 * 
 */

class UImage;
class UTextBlock;
class UHorizontalBox;
class ATurnBasedCharacter;

DECLARE_DELEGATE_OneParam(FOnSkillIconMouseEnter, FGameplayTag);
DECLARE_DELEGATE_OneParam(FOnSkillIconMouseLeave, FGameplayTag);
DECLARE_DELEGATE(FOnSkillIconClicked);
DECLARE_DELEGATE_OneParam(FOnTargetHovered, AActor*);
DECLARE_DELEGATE_TwoParams(FOnTargetClicked, AActor*, FKey);

UCLASS()
class DEMO_API UUW_TBSelect_SkillIcon : public UUserWidget
{
	GENERATED_BODY()
public:
protected:
public:
	virtual void NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent);
	virtual void NativeOnMouseLeave(const FPointerEvent& InMouseEvent);
	virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent);

	//property
private:
	FGameplayTag SkillTag;
	float TimeRemaining;
	float CooldownDuration;
protected:
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
		UImage* SkillIcon;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
		UImage* CooldownRateCover;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
		UTextBlock* TimeRemainingText;

public:
	FOnSkillIconMouseEnter OnSkillIconMouseEnter;
	FOnSkillIconMouseLeave OnSkillIconMouseLeave;
	FOnSkillIconClicked OnSkillIconClicked;

	//function
private:
protected:
public:
	void Refresh(FGameplayTag NewSkillTag, float TimeRemaining, float CooldownDuration);
};

UCLASS()
class DEMO_API UUW_TBSelect : public UUserWidget
{
	GENERATED_BODY()
public:
protected:
public:
	virtual void NativeConstruct()override;

	//property
private:
	ATurnBasedCharacter* TurnCharacter;

	TArray<UUW_TBSelect_SkillIcon*> Icons;
protected:
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
		UHorizontalBox* SkillIconContainer;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
		TSubclassOf<UUW_TBSelect_SkillIcon> SkillIconClass;
public:
	FOnSkillIconMouseEnter OnSkillIconMouseEnter;
	FOnSkillIconMouseLeave OnSkillIconMouseLeave;
	FOnSkillIconClicked OnSkillIconClicked;

	FOnTargetHovered OnTargetHovered;
	FOnTargetClicked OnTargetClicked;

	//function
private:
	void CreateIcon();
	UFUNCTION()void ExecuteOnSkillIconMouseEnter(FGameplayTag InSkillTag);
	UFUNCTION()void ExecuteOnSkillIconMouseLeave(FGameplayTag InSkillTag);
	UFUNCTION()void ExecuteOnSkillIconClicked();
	UFUNCTION()void ExecuteOnTargetHovered(AActor* InTarget);
	UFUNCTION()void ExecuteOnTargetClicked(AActor* InTarget);
protected:
public:
	void Activate(ATurnBasedCharacter* NewTurnCharacter);
};

/*
* 스킬아이콘 최대 5개, 상황에 따라 숨기고 보여주고 다름
* 
* 
* 턴캐릭터가 바뀌면 아이콘 데이터도 바뀜
* 
* 
* 
* 아이콘 클릭시 카메라 위치 변경
* onskilliconclicked->ChangeSelectCameraView()
* 
* 타겟클릭시 현재 skilltag와 함께 playsequence
* 
*/