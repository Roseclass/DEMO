#include "Characters/TurnBasedCameraComponent.h"
#include "Global.h"

#include "GameplayTagsManager.h"

UTurnBasedCameraComponent::UTurnBasedCameraComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UTurnBasedCameraComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UTurnBasedCameraComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void UTurnBasedCameraComponent::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	SkillCameraTransform.Empty();

	UGameplayTagsManager& Manager = UGameplayTagsManager::Get();
	TSharedPtr<FGameplayTagNode> tagNode = Manager.FindTagNode(SkillRootTag);

	if (!tagNode.IsValid())return;
	for (auto i : tagNode->GetChildTagNodes())
		SkillCameraTransform.FindOrAdd(i->GetCompleteTag());
}
