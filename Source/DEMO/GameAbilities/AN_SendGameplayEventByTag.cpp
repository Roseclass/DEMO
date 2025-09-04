#include "GameAbilities/AN_SendGameplayEventByTag.h"
#include "Global.h"
#include "GameFramework/Character.h"

#include "GameAbilities/AbilityComponent.h"

FString UAN_SendGameplayEventByTag::GetNotifyName_Implementation() const
{
	return EventTag.GetTagName().ToString();
}

void UAN_SendGameplayEventByTag::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);

	// for editor preview
	CheckNull(MeshComp);
	CheckNull(MeshComp->GetOwner());
	ACharacter* owner = Cast<ACharacter>(MeshComp->GetOwner());
	CheckNull(owner);

	UAbilityComponent* asc = CHelpers::GetComponent<UAbilityComponent>(owner);
	CheckNull(asc);
	FGameplayEventData data;
	asc->HandleGameplayEvent(EventTag, &data);
}
