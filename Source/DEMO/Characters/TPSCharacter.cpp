#include "Characters/TPSCharacter.h"
#include "Global.h"

#include "Components/InputComponent.h"

#include "GameFramework/CharacterMovementComponent.h"

#include "DEMOGameState.h"

#include "GameAbilities/AbilityComponent.h"
#include "GameAbilities/AttributeSet_Character.h"

#include "Objects/EventTrigger.h"

ATPSCharacter::ATPSCharacter()
{
	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...	
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f); // ...at this rotation rate

	// Note: For faster iteration times these variables, and many more, can be tweaked in the Character Blueprint
	// instead of recompiling to adjust them
	GetCharacterMovement()->JumpZVelocity = 700.f;
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->MaxWalkSpeed = 500.f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;
}
void ATPSCharacter::BeginPlay()
{
	Super::BeginPlay();
}

void ATPSCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ATPSCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// Set up gameplay key bindings
	check(PlayerInputComponent);
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);
	PlayerInputComponent->BindAction("Action", IE_Pressed, this, &ATPSCharacter::Action);

	PlayerInputComponent->BindAxis("MoveForward", this, &ATPSCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &ATPSCharacter::MoveRight);

	PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
}

FString ATPSCharacter::GetUniqueSaveName()
{
	return FString();
}

void ATPSCharacter::OnBeforeSave(USaveGameData* SaveData)
{
	GetUniqueSaveName();
}

void ATPSCharacter::OnAfterLoad(USaveGameData* ReadData)
{
	//TODO
}

AEventTrigger* ATPSCharacter::GetEventTrigger()
{
	return EventTrigger;
}

void ATPSCharacter::EnableTriggerCollision(bool bEnable)
{
	CheckTrue_Print(!EventTrigger, "EventTrigger is nullptr");

	bEnable ? EventTrigger->Activate() : EventTrigger->Deactivate();
}

void ATPSCharacter::Action()
{
	CheckTrue(!RuntimeData.bInitComplete);
	Ability->TryActivateAbilitiesByTag(FGameplayTagContainer(RuntimeData.ActionTag));
}

void ATPSCharacter::MoveForward(float Value)
{
	if ((Controller != nullptr) && (Value != 0.0f))
	{
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		AddMovementInput(Direction, Value);
	}
}

void ATPSCharacter::MoveRight(float Value)
{
	if ((Controller != nullptr) && (Value != 0.0f))
	{
		// find out which way is right
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get right vector 
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		// add movement in that direction
		AddMovementInput(Direction, Value);
	}
}

void ATPSCharacter::Init(UPrimaryDataAsset* DA)
{
	Super::Init(DA);

	UTPSCharacterData* tpsData = Cast<UTPSCharacterData>(DA);
	CheckTrue_Print(!tpsData, "tpsData cast Fail!!");
	RuntimeData = tpsData->RuntimeData;
	RuntimeData.bInitComplete = 1;
	GetMesh()->SetSkeletalMesh(tpsData->SkeletalMesh.Get());
	GetMesh()->SetAnimationMode(EAnimationMode::AnimationBlueprint);
	GetMesh()->SetAnimInstanceClass(tpsData->AnimBlueprint.Get());

	//asc
	TArray<FAbilitySpecInfo> abilities = tpsData->GrantedAbilities;
	for (auto& i : abilities)i.SourceObject = tpsData;
	Ability->Init(abilities);

	//Trigger
	EventTrigger = GetWorld()->SpawnActorDeferred<AEventTrigger>(tpsData->ActionTrigger, FTransform());
	EventTrigger->OnBeginOverlap.AddLambda([this](FGameplayTag EventTag, const FGameplayEventData* Payload)
		{
			//EventTag = FGameplayTag::RequestGameplayTag(
			//	FName(
			//		RuntimeData.ActionTag.ToString() + 
			//		TEXT(".") + 
			//		RuntimeData.ActionOnBeginOverlapLeaf.ToString()
			//	));
			//Ability->HandleGameplayEvent(EventTag, Payload);

			ADEMOGameState* gs = Cast<ADEMOGameState>(UGameplayStatics::GetGameState(GetWorld()));
			gs->RequestPhaseChange(EGameStatePhase::TurnBased, nullptr);
		});
	EventTrigger->OnEndOverlap;
	UGameplayStatics::FinishSpawningActor(EventTrigger, FTransform());
	EventTrigger->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, tpsData->ActionSocketName);
	if (USceneComponent* Root = EventTrigger->GetRootComponent())
		Root->SetRelativeLocationAndRotation(FVector::ZeroVector, FRotator::ZeroRotator);
	EventTrigger->SetOwner(this);


	/*
	* TODO softptr asset init
	*/
}