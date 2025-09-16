#include "Characters/BaseCharacter.h"
#include "Global.h"

#include "Components/CapsuleComponent.h"

#include "GameAbilities/AbilityComponent.h"
#include "GameAbilities/AttributeSet_Character.h"


ABaseCharacter::ABaseCharacter()
{
	//actor
	CHelpers::CreateActorComponent<UAbilityComponent>(this, &Ability, "Ability");
	AttributeSet = CreateDefaultSubobject<UAttributeSet_Character>(TEXT("AttributeSet"));

	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	GetMesh()->SetRelativeLocation(FVector(0, 0, -97));
	GetMesh()->SetRelativeRotation(FQuat(FRotator3d(0, -90, 0)));
}

void ABaseCharacter::BeginPlay()
{
	Super::BeginPlay();
}

void ABaseCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ABaseCharacter::SetGenericTeamId(const FGenericTeamId& TeamID)
{
	GenericTeamId = TeamID;
}

FGenericTeamId ABaseCharacter::GetGenericTeamId() const
{
	return GenericTeamId;
}

UAbilitySystemComponent* ABaseCharacter::GetAbilitySystemComponent() const
{
	return Ability;
}

FGuid ABaseCharacter::GetUniqueSaveName() const
{
	return UniqueSaveName;
}

void ABaseCharacter::OnBeforeSave(USaveGameData* SaveData)
{

}

void ABaseCharacter::OnAfterLoad(USaveGameData* ReadData)
{

}

void ABaseCharacter::Init(FGuid NewSaveName, UPrimaryDataAsset* DA)
{
	UniqueSaveName = NewSaveName;
}

FGameplayTag ABaseCharacter::GetDataTag() const
{
	return FGameplayTag();
}
TArray<FGameplayTag> ABaseCharacter::GetDataTags() const
{
	return TArray<FGameplayTag>();
}
