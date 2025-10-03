#include "DEMOPlayerController.h"
#include "Global.h"

#include "Datas/UITypes.h"

#include "Widgets/UW_SkillSlotAssigner.h"

ADEMOPlayerController::ADEMOPlayerController()
{

}

void ADEMOPlayerController::BeginPlay()
{
	Super::BeginPlay();

	SkillSlotAssigner = CreateWidget<UUW_SkillSlotAssigner>(GetWorld(), SkillSlotAssignerClass);
	if (SkillSlotAssigner)
		SkillSlotAssigner->AddToViewport();	
}

void ADEMOPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	InputComponent->BindAction("Skill", IE_Pressed, this, &ADEMOPlayerController::ToggleSkillSlotAssigner);
}

void ADEMOPlayerController::ToggleSkillSlotAssigner()
{
	if (SkillSlotAssigner->IsVisible())
	{
		SkillSlotAssigner->Hide();
		SetInputMode(FInputModeGameOnly());
		SetShowMouseCursor(1);
	}
	else 
	{
		SkillSlotAssigner->Show();
		SetInputMode(FInputModeGameAndUI());
		SetShowMouseCursor(0);
	}
}

const FDEMOSkillUIDatas* ADEMOPlayerController::FindUIDatasByTag(FGameplayTag InSkillRootTag) const
{
	if (!UIDataRegistry->SkillUIDataMap.Contains(InSkillRootTag))
	{
		CLog::Print("ADEMOPlayerController::FindUIDatasByTag InSkillRootTag is not contains");
		return nullptr;
	}
	return &UIDataRegistry->SkillUIDataMap[InSkillRootTag];
}

const FDEMOSkillUIData* ADEMOPlayerController::FindUIDataByTag(FGameplayTag InSkillTag) const
{
	for (auto& Tuple : UIDataRegistry->SkillUIDataMap)
	{
		for (auto& Data : Tuple.Value.Datas)
			if (Data.SkillTag == InSkillTag)return &Data;
	}

	CLog::Print("ADEMOPlayerController::FindUIDataByTag InSkillTag is not contains");
	return nullptr;
}

UObject* ADEMOPlayerController::GetEmptyRoundIconResourceObject() const
{
	return UIDataRegistry->EmptyRoundIconResourceObject;
}
