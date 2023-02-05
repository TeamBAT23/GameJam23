// Copyright Epic Games, Inc. All Rights Reserved.

#include "RootCharacter.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/SpringArmComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "GameplayAbilitySystem/RootAbilitySystemComponent.h"


//////////////////////////////////////////////////////////////////////////
// AGGJ23Character

ARootCharacter::ARootCharacter()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);
		
	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

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

	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character) 
	// are set in the derived blueprint asset named ThirdPersonCharacter (to avoid direct content references in C++)

	//Additional setup needed to support GAS
	AbilitySystemComponent = CreateDefaultSubobject<URootAbilitySystemComponent>("AbilitySystemComponent");
	AbilitySystemComponent->SetIsReplicated(true);
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Minimal);
}

UAbilitySystemComponent* ARootCharacter::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

void ARootCharacter::BeginPlay()
{
	// Call the base class  
	Super::BeginPlay();
}

//////////////////////////////////////////////////////////////////////////
// Input

void ARootCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
}

void ARootCharacter::Server_GASInit()
{
	AbilitySystemComponent->InitAbilityActorInfo(this, this);
}

void ARootCharacter::Client_GASInit()
{
	AbilitySystemComponent->InitAbilityActorInfo(this, this);

	BindAbilityComponentToInputComponent();
}

void ARootCharacter::BindAbilityComponentToInputComponent() const
{
	//Called from client ability component init as well as input component init.
	if(AbilitySystemComponent && InputComponent)
	{
		const FGameplayAbilityInputBinds Binds{
			GET_ENUMERATOR_NAME_CHECKED(ERootAbilityInputId, Confirm).ToString(),
			GET_ENUMERATOR_NAME_CHECKED(ERootAbilityInputId, Cancel).ToString(),
			"ERootAbilityInputId",
			static_cast<int32>(ERootAbilityInputId::Confirm),
			static_cast<int32>(ERootAbilityInputId::Cancel)
		};

		AbilitySystemComponent->BindAbilityActivationToInputComponent(InputComponent, Binds);
	}
}

void ARootCharacter::Move(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D MovementVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	
		// get right vector 
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		// add movement 
		AddMovementInput(ForwardDirection, MovementVector.Y);
		AddMovementInput(RightDirection, MovementVector.X);
	}
}

void ARootCharacter::Look(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// add yaw and pitch input to controller
		AddControllerYawInput(LookAxisVector.X);
		AddControllerPitchInput(LookAxisVector.Y);
	}
}




