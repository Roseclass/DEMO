#include "Characters/TurnBasedAnimBlueprint.h"
#include "Global.h"

#include "GameplayTagsManager.h"

#include "Characters/TurnBasedCharacter.h"

#include "GameAbilities/AbilityComponent.h"

void UTurnBasedAnimBlueprint::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();
	OwnerCharacter = Cast<ATurnBasedCharacter>(TryGetPawnOwner());
	CheckNull(OwnerCharacter);
	OwnerCharacter->OnAnimTagChanged.AddUFunction(this, "ChangeSkillTag");
}

void UTurnBasedAnimBlueprint::NativeBeginPlay()
{
	Super::NativeBeginPlay();
}

void UTurnBasedAnimBlueprint::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

#if WITH_EDITOR
	if (!GetWorld() || GetWorld()->WorldType == EWorldType::EditorPreview)
	{
		// 프리뷰 전용 값 사용
		CurrentSkillTag = PreviewSkillTag;
		return;
	}
#endif
}

void UTurnBasedAnimBlueprint::ChangeSkillTag(FGameplayTag NewSkillTag)
{
	CurrentSkillTag = NewSkillTag;
}