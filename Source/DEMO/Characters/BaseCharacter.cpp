#include "Characters/BaseCharacter.h"
#include "Global.h"

#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/SpringArmComponent.h"

#include "SaveLoadSubsystem.h"

#include "GameAbilities/AbilityComponent.h"
#include "GameAbilities/AttributeSet_Character.h"


ABaseCharacter::ABaseCharacter()
{
	//actor
	CHelpers::CreateActorComponent<UAbilityComponent>(this, &Ability, "Ability");
	AttributeSet = CreateDefaultSubobject<UAttributeSet_Character>(TEXT("AttributeSet"));

	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	// set our turn rate for input
	TurnRateGamepad = 50.f;

	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 400.0f; // The camera follows at this distance behind the character	
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	GetMesh()->SetRelativeLocation(FVector(0, 0, -97));
	GetMesh()->SetRelativeRotation(FQuat(FRotator3d(0, -90, 0)));

	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character) 
	// are set in the derived blueprint asset named ThirdPersonCharacter (to avoid direct content references in C++)
}

void ABaseCharacter::BeginPlay()
{
	Super::BeginPlay();
}

void ABaseCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

FGenericTeamId ABaseCharacter::GetGenericTeamId() const
{
	return TeamID;
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
