#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GameAbilities/AbilityUIEnums.h"
#include "GameplayTagContainer.h"
#include "UW_TurnBased_Select.generated.h"

/**
 * ws skill select
 * 캐릭터가 가진 스킬셋을 보여주고 선택해서 정해진 함수를 broadcast
 * 
 * ad target select
 * 현재 어떤 캐릭터를 선택했는지 보여주고 선택해서 정해진 함수를 broadcast
 *
 */

class UImage;
class UScaleBox;
class USizeBox;
class UTextBlock;
class URichTextBlock;
class UWidgetAnimation;
class UUMGSequencePlayer;
class ATurnBasedCharacter;
class ASelectWidgetActor;
class UDEMOSkillUIDataRegistry;

DECLARE_MULTICAST_DELEGATE_TwoParams(FOnConfirm, FGameplayTag, ATurnBasedCharacter*);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnHorizontalMove, ATurnBasedCharacter*);
DECLARE_DELEGATE_RetVal(bool, FCanAcceptKey);

USTRUCT()
struct FSelectSkillRowData
{
	GENERATED_BODY()
public:
	FSelectSkillRowData() {};
	FSelectSkillRowData(FGameplayTag InTag, ESkillTargetType InType, int32 InRemainCd, int32 InCd)
		: SkillTag(InTag), TargetType(InType), RemainCd(InRemainCd), Cd(InCd){};
public:
	FGameplayTag SkillTag;
	ESkillTargetType TargetType;
	int32 RemainCd;
	int32 Cd;
};

UCLASS()
class DEMO_API UUW_TurnBased_SelectSkillRow : public UUserWidget
{
	GENERATED_BODY()
public:
protected:
public:

	//property
private:
	bool bIsCoolDown;
	UPROPERTY()UUMGSequencePlayer* CurrentPlayer;
	FSelectSkillRowData RowData;
protected:
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
		UScaleBox* ScaleBox;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
		USizeBox* SizeBox;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
		UImage* Background;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
		UImage* Cover;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
		UImage* CooltimeCover;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
		UTextBlock* CooltimeLeft;

	UPROPERTY(BlueprintReadOnly, Transient, meta = (BindWidgetAnim))
		UWidgetAnimation* FocusAnim;
public:

	//function
private:
protected:
public:
	void Refresh(FSelectSkillRowData InData);
	void OnFocus();
	void OffFocus();
	void OnFocusWithoutAnim();
	void OffFocusWithoutAnim();

	FORCEINLINE FGameplayTag GetSkillTag()const { return RowData.SkillTag; }
	FORCEINLINE FSelectSkillRowData GetRowData()const { return RowData; }
	FORCEINLINE bool IsCoolDown()const { return bIsCoolDown; }
};

UCLASS()
class DEMO_API UUW_TurnBased_Select : public UUserWidget
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
	//skill
	ASelectWidgetActor* OwningActor;
	ATurnBasedCharacter* TurnCharacter;
	ESkillSlotLocation CursorLocation = ESkillSlotLocation::Attack;
	TArray<UUW_TurnBased_SelectSkillRow*> Icons;
	TObjectPtr<UDEMOSkillUIDataRegistry> UIDataRegistry;

	//target
	bool bActive;
	ASelectWidgetActor* TargetCursorActor;
	int32 TargetIndex;
	int32 PlayerStartIndex;
	int32 EnemyStartIndex;
	TArray<ATurnBasedCharacter*> TargetArray; // Player~Enemy
	TArray<ATurnBasedCharacter*> PlayerArray;
	TArray<ATurnBasedCharacter*> EnemyArray;
protected:
	UPROPERTY(BlueprintReadOnly, Category = "Slot", meta = (BindWidget))
		UUW_TurnBased_SelectSkillRow* Icon_Attack;

	UPROPERTY(BlueprintReadOnly, Category = "Slot", meta = (BindWidget))
		UUW_TurnBased_SelectSkillRow* Icon_Slot0;

	UPROPERTY(BlueprintReadOnly, Category = "Slot", meta = (BindWidget))
		UUW_TurnBased_SelectSkillRow* Icon_Slot1;

	UPROPERTY(BlueprintReadOnly, Category = "Slot", meta = (BindWidget))
		UUW_TurnBased_SelectSkillRow* Icon_Passive;

	UPROPERTY(BlueprintReadOnly, Category = "Slot", meta = (BindWidget))
		URichTextBlock* SkillDescription;

	UPROPERTY(BlueprintReadOnly, Category = "Slot", meta = (BindWidget))
		UTextBlock* SkillName;
public:
	FOnConfirm OnConfirm;
	FOnHorizontalMove OnHorizontalMove;
	FCanAcceptKey CanAcceptKey;

	//function
private:
	//skill
	void ChangeCursorLocation(ESkillSlotLocation NewLocation, bool Anim = 1);
	UFUNCTION()void Up();
	UFUNCTION()void Down();

	//target
	UFUNCTION()void Left();
	UFUNCTION()void Right();

	UFUNCTION()void Confirm();
	bool CheckTargetType();
	void SetWidgetDatas(ATurnBasedCharacter* NewTargetCharacter);

protected:
public:
	void SetOwningActor(ASelectWidgetActor* NewOwningActor);
	void SetTargetCursorActor(ASelectWidgetActor* NewTargetCursorActor);
	void Activate(ATurnBasedCharacter* NewCurrentTurnCharacter, const TArray<ATurnBasedCharacter*>& InPlayerArray, const TArray<ATurnBasedCharacter*>& InEnemyArray);

	FGameplayTag GetCurrentSkillTag()const;
	TArray<FGameplayTag> GetAllSkillTags()const;

	ATurnBasedCharacter* GetCurrentTarget()const;
	TArray<ATurnBasedCharacter*> GetTargetArray()const;
	TArray<ATurnBasedCharacter*> GetPlayerArray()const;
	TArray<ATurnBasedCharacter*> GetEnemyArray()const;
};
