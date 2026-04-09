#include "Characters/AI/TurnBasedEnemy.h"
#include "Global.h"
#include "Components/SceneComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/WidgetComponent.h"

#include "BehaviorTree/BehaviorTree.h"

#include "Characters/AI/TurnBasedAIController.h"
#include "Characters/AI/TurnBasedBehaviorComponent.h"

#include "GameAbilities/AbilityComponent.h"
#include "GameAbilities/AttributeSet_Character.h"

#include "Widgets/UW_TurnBased_Select.h"

ATurnBasedEnemy::ATurnBasedEnemy()
{
	PrimaryActorTick.bCanEverTick = true;
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
	AIControllerClass = ATurnBasedAIController::StaticClass();
}

void ATurnBasedEnemy::BeginPlay()
{
	Super::BeginPlay();
}

void ATurnBasedEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ATurnBasedEnemy::InitAssets(UPrimaryDataAsset* DA)
{
	Super::InitAssets(DA);
}

void ATurnBasedEnemy::InitGA(UPrimaryDataAsset* DA)
{
	UTurnBasedCharacterData* turnbasedData = Cast<UTurnBasedCharacterData>(DA);
	CheckTrue_Print(!turnbasedData, "turnbasedData cast Fail!!");

	//runtimedata
	RuntimeData = turnbasedData->RuntimeData;
	RuntimeData.bInitComplete = 1;
	for (int32 i = 0; i < int32(ESkillSlotLocation::MAX); i++)
		RuntimeData.EquippedSkillTags[i] = turnbasedData->AIEquippedSkillTags[i];

	//asc
	TArray<FAbilitySpecInfo> abilities;
	for (auto tag : turnbasedData->AIEquippedSkillTags)
	{
		if (!turnbasedData->AvailableAbilities.Contains(tag))continue;
		if (!turnbasedData->AvailableAbilities[tag].AbilityClass)continue;
		abilities.Add(turnbasedData->AvailableAbilities[tag]);
	}
	for (auto& tuple : turnbasedData->GrantedAbilities)
	{
		if (!tuple.Value.AbilityClass)continue;
		abilities.Add(tuple.Value);
	}
	for (auto& i : abilities)i.SourceObject = turnbasedData;
	Ability->InitGA(abilities);
	Ability->InitAttributes(&turnbasedData->AttributeInitialInfos);
}

void ATurnBasedEnemy::Init(FGuid NewSaveName, UPrimaryDataAsset* DA)
{
	Super::Init(NewSaveName, DA);

	UTurnBasedCharacterData* turnbasedData = Cast<UTurnBasedCharacterData>(DA);
	CheckTrue_Print(!turnbasedData, "turnbasedData cast Fail!!");

	ATurnBasedAIController* controller = Cast<ATurnBasedAIController>(GetController());
	UBehaviorTree* bt = turnbasedData->BehaviorTree.LoadSynchronous();
	controller->Init(bt);
}

void ATurnBasedEnemy::HoverSkillIcon(FGameplayTag InTag)
{
	OnHoverSkillIcon.ExecuteIfBound(InTag);
}

void ATurnBasedEnemy::ClickSkillIcon(FGameplayTag InTag)
{
	OnClickSkillIcon.ExecuteIfBound(InTag);
}

void ATurnBasedEnemy::HoverTarget(AActor* InTarget)
{
	OnHoverTarget.ExecuteIfBound(InTarget);
}

void ATurnBasedEnemy::ClickTarget(AActor* InTarget)
{
	OnClickTarget.ExecuteIfBound(InTarget);
}

void ATurnBasedEnemy::LinkSelectWidget(UUW_TBSelect* NewSelectWidget)
{
	SelectWidget = NewSelectWidget;
}
