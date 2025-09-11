#include "Characters/TurnBasedCharacter.h"
#include "Global.h"

#include "GameAbilities/AbilityComponent.h"
#include "GameAbilities/AttributeSet_Character.h"

ATurnBasedCharacter::ATurnBasedCharacter()
{

}

void ATurnBasedCharacter::BeginPlay()
{
	Super::BeginPlay();
}

void ATurnBasedCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	CLog::Print(GetActorLocation(), -1, 0, FColor::Black);
}

void ATurnBasedCharacter::Init(FGuid NewSaveName, UPrimaryDataAsset* DA)
{
	Super::Init(NewSaveName, DA);

	UTurnBasedCharacterData* turnbasedData = Cast<UTurnBasedCharacterData>(DA);
	CheckTrue_Print(!turnbasedData, "turnbasedData cast Fail!!");
	RuntimeData.bInitComplete = 1;
	RuntimeData = turnbasedData->RuntimeData;
	GetMesh()->SetSkeletalMesh(turnbasedData->SkeletalMesh.Get());
	GetMesh()->SetAnimationMode(EAnimationMode::AnimationBlueprint);
	GetMesh()->SetAnimInstanceClass(turnbasedData->AnimBlueprint.Get());

	//asc
	TArray<FAbilitySpecInfo> abilities = turnbasedData->GrantedAbilities;
	for (auto& i : abilities)i.SourceObject = turnbasedData;
	Ability->Init(abilities);
}
