#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GameAbilities/AbilityUIEnums.h"
#include "GameplayTagContainer.h"
#include "UW_SelectSkill.generated.h"

/**
 * wasd + space로 조종
 * 캐릭터가 가진 스킬셋을 보여주고 선택해서 정해진 함수를 broadcast
 * 
 * 0	1
 * 2	3
 * 4	5
 *	back
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

DECLARE_MULTICAST_DELEGATE(FOnBackDown);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnConfirmDown, FGameplayTag);

//#define DECLARE_SLOT_WIDGETS(SlotIndex) \
//UPROPERTY(BlueprintReadOnly, Category = "Slot" #SlotIndex, meta = (BindWidget)) \
//UImage* Slot##SlotIndex##_Background; \
//\
//UPROPERTY(BlueprintReadOnly, Category = "Slot" #SlotIndex, meta = (BindWidget)) \
//URichTextBlock* Slot##SlotIndex##_RichText; \
//\
//UPROPERTY(BlueprintReadOnly, Category = "Slot" #SlotIndex, meta = (BindWidget)) \
//UTextBlock* Slot##SlotIndex##_ResourceText;

UCLASS()
class DEMO_API UUW_SelectSkillIcon : public UUserWidget
{
	GENERATED_BODY()
public:
protected:
public:

	//property
private:
	UPROPERTY()UUMGSequencePlayer* CurrentPlayer;
	FGameplayTag SkillTag;
protected:
	UPROPERTY(BlueprintReadOnly,  meta = (BindWidget))
		UScaleBox* ScaleBox;

	UPROPERTY(BlueprintReadOnly,  meta = (BindWidget))
		USizeBox* SizeBox;

	UPROPERTY(BlueprintReadOnly,  meta = (BindWidget))
		UImage* Background;

	UPROPERTY(BlueprintReadOnly,  meta = (BindWidget))
		UImage* Cover;

	UPROPERTY(BlueprintReadOnly, Transient, meta = (BindWidgetAnim))
		UWidgetAnimation* FocusAnim;
public:

	//function
private:
protected:
public:
	void Refresh(FGameplayTag InTag);
	void OnFocus();
	void OffFocus();
	
	FORCEINLINE FGameplayTag GetTag()const { return SkillTag; }
};

UCLASS()
class DEMO_API UUW_SelectSkill : public UUserWidget
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
	ASelectWidgetActor* OwningActor;
	ATurnBasedCharacter* TargetCharacter;
	ESkillSlotLocation CursorLocation = ESkillSlotLocation::Attack;
	TArray<UUW_SelectSkillIcon*> Icons;
	TObjectPtr<UDEMOSkillUIDataRegistry> UIDataRegistry;
protected:
	UPROPERTY(BlueprintReadOnly, Category = "Slot", meta = (BindWidget)) 
		UUW_SelectSkillIcon* Icon_Attack;

	UPROPERTY(BlueprintReadOnly, Category = "Slot", meta = (BindWidget)) 
		UUW_SelectSkillIcon* Icon_Slot0;

	UPROPERTY(BlueprintReadOnly, Category = "Slot", meta = (BindWidget)) 
		UUW_SelectSkillIcon* Icon_Slot1;

	UPROPERTY(BlueprintReadOnly, Category = "Slot", meta = (BindWidget)) 
		UUW_SelectSkillIcon* Icon_Passive;

	UPROPERTY(BlueprintReadOnly, Category = "Slot", meta = (BindWidget))
		URichTextBlock* SkillDescription; 

	UPROPERTY(BlueprintReadOnly, Category = "Slot", meta = (BindWidget))
		UTextBlock* SkillName;		
public:
	FOnConfirmDown OnConfirmDown;

	//function
private:
	void ChangeCursorLocation(int32 Offset);
	void Up();
	void Down();
	void Confirm();
	void SetWidgetDatas(ATurnBasedCharacter* NewTargetCharacter);
protected:
public:
	void SetOwningActor(ASelectWidgetActor* NewOwningActor);
	void Activate(ATurnBasedCharacter* NewCurrentTurnCharacter);
};

/*
* 캐릭터에서 나오는 데이터로 위젯 갱신
* 캐릭터의 신컴포넌트에 위치하면됨
* 
* 프레임, 백그라운드, 리치 텍스트블록, 자원소모 백그라운드, 자원소모 텍스트
* 
* 데이터애샛과 테이블 관리의 차이점은 뭘까?
*/