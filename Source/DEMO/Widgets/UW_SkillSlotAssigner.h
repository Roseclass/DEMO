#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "SaveLoad/SaveLoadTypes.h"
#include "UW_SkillSlotAssigner.generated.h"

/**
 * Only For TPSPhase
 */

enum class ESkillSlotLocation : uint8;
enum class ESkillListLocation : uint8;

class UButton;
class UImage;
struct FDEMOSkillUIDatas;
struct FDEMOSkillUIData;

class UUW_SkillSlotAssignerPopup;
class UUW_SkillSlotAssignerIcon;
class UUW_SkillSlotAssigner;

DECLARE_MULTICAST_DELEGATE(FSkillSlotAssignerSignature);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnSkillSlotAssignerDataSignature, FGameplayTag);
DECLARE_MULTICAST_DELEGATE_TwoParams(FSkillSlotAssignerSlotSelectorClicked, ESkillSlotLocation, FGameplayTag);

UCLASS()
class DEMO_API UUW_SkillSlotAssignerSlotSelector : public UUserWidget
{
	GENERATED_BODY()
public:
protected:
public:
	virtual void NativeOnInitialized() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

	//property
private:
	FGameplayTag SkillTag;
	UButton* Buttons[int32(ESkillSlotLocation::MAX)];
protected:
	//Attack
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
		UButton* Button_Attack;

	//Skill_0
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
		UButton* Button_Skill_0;

	//Skill_1
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
		UButton* Button_Skill_1;

	//Passive
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
		UButton* Button_Passive;
public:
	FSkillSlotAssignerSlotSelectorClicked OnButtonClicked;
	//function
private:
	UFUNCTION()void OnAttackClicked();
	UFUNCTION()void OnSkill_0Clicked();
	UFUNCTION()void OnSkill_1Clicked();
	UFUNCTION()void OnPassiveClicked();
protected:
public:
	void Show(FGameplayTag NewTag);
	void Hide();
};

UCLASS()
class DEMO_API UUW_SkillSlotAssignerPopup : public UUserWidget
{
	GENERATED_BODY()
public:
protected:
public:

	//property
private:
	FGameplayTag SkillTag;
protected:
public:

	//function
private:
	void Refresh();
protected:
public:
	void Show(FGameplayTag NewTag);
	void Hide();
};

UCLASS()
class DEMO_API UUW_SkillSlotAssignerIcon : public UUserWidget
{
	GENERATED_BODY()
public:
protected:
public:
	virtual void NativeOnInitialized() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

	//property
private:
	bool bIsPopupOpen;
	FGameplayTag SkillTag;
protected:
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
		UImage* Background;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
		UButton* Button;

public:
	FOnSkillSlotAssignerDataSignature OnClicked;
	FOnSkillSlotAssignerDataSignature OnMouseEnter;
	FSkillSlotAssignerSignature OnMouseLeave;
	//function
private:
	UFUNCTION()void OnButtonClicked();
protected:
public:
	void Refresh(FGameplayTag NewTag);

	FORCEINLINE FGameplayTag GetSkillTag()const { return SkillTag; }
};

UCLASS()
class DEMO_API UUW_SkillSlotAssigner : public UUserWidget
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
	TArray<FGameplayTag> CharacterSkillRootTags;
	int32 CharacterSkillRootTagIndex;
	TMap<FGameplayTag, FSaveUIData> CachedUISaveDatas;

	UUW_SkillSlotAssignerIcon* ListIcons[int32(ESkillListLocation::MAX)];
	UUW_SkillSlotAssignerIcon* SlotIcons[int32(ESkillSlotLocation::MAX)];
protected:
	//IdxControl
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
		UButton* IndexToLeft;

	//IdxControl
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
		UButton* IndexToRight;

	//Description
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
		UUW_SkillSlotAssignerPopup* DescriptionPopup;

	//Selector
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
		UUW_SkillSlotAssignerSlotSelector* SlotSelector;

	//Skill_00
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
		UUW_SkillSlotAssignerIcon* ListIcon_00;

	//Skill_01
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
		UUW_SkillSlotAssignerIcon* ListIcon_01;

	//Skill_10
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
		UUW_SkillSlotAssignerIcon* ListIcon_10;

	//Skill_11
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
		UUW_SkillSlotAssignerIcon* ListIcon_11;

	//Skill_20
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
		UUW_SkillSlotAssignerIcon* ListIcon_20;

	//Skill_21
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
		UUW_SkillSlotAssignerIcon* ListIcon_21;

	//Skill_30
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
		UUW_SkillSlotAssignerIcon* ListIcon_30;

	//Skill_31
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
		UUW_SkillSlotAssignerIcon* ListIcon_31;

	//Attack
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
		UUW_SkillSlotAssignerIcon* SlotIcon_Attack;

	//Skill_0
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
		UUW_SkillSlotAssignerIcon* SlotIcon_Skill_0;

	//Skill_1
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
		UUW_SkillSlotAssignerIcon* SlotIcon_Skill_1;

	//Passive
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
		UUW_SkillSlotAssignerIcon* SlotIcon_Passive;

public:

	//function
private:
	UFUNCTION()void ChangeCurrentCharacterData(bool IsLeft);
	UFUNCTION()void ChangeSlotData(ESkillSlotLocation InLocation, FGameplayTag InTag);
	UFUNCTION()void OpenDescriptionPopup(FGameplayTag InTag);
	UFUNCTION()void CloseDescriptionPopup();
	UFUNCTION()void OpenSlotSelector(FGameplayTag InTag);
	UFUNCTION()void CloseSlotSelector();
	void Refresh(const FDEMOSkillUIDatas* Datas);
	void UpdateCachedUISaveDatas();
protected:
public:
	void Init(AActor* Manager, AActor* Camera);
	void Show();
	void Hide();
};

/*


*/