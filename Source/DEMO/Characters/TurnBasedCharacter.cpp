#include "Characters/TurnBasedCharacter.h"
#include "Global.h"

#include "Components/InputComponent.h"

#include "GameFramework/CharacterMovementComponent.h"

#include "GameAbilities/AbilityComponent.h"
#include "GameAbilities/AttributeSet_Character.h"

#include "Objects/EventTrigger.h"

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
}

FString ATurnBasedCharacter::GetUniqueSaveName()
{
	return FString();
}

void ATurnBasedCharacter::OnBeforeSave(USaveGameData* SaveData)
{

}

void ATurnBasedCharacter::OnAfterLoad(USaveGameData* ReadData)
{

}

