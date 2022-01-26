// Copyright Epic Games, Inc. All Rights Reserved.

#include "RLGLCharacter.h"
#include "RLGLProjectile.h"
#include "Animation/AnimInstance.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/InputSettings.h"
#include "HeadMountedDisplayFunctionLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "TimerManager.h"
#include "Kismet/KismetSystemLibrary.h"

DEFINE_LOG_CATEGORY_STATIC(LogFPChar, Warning, All);

//////////////////////////////////////////////////////////////////////////
// ARLGLCharacter

ARLGLCharacter::ARLGLCharacter()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(55.f, 96.0f);

	// set our turn rates for input
	BaseTurnRate = 45.f;
	BaseLookUpRate = 45.f;

	// Create a CameraComponent
	FirstPersonCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
	FirstPersonCameraComponent->SetupAttachment(GetCapsuleComponent());
	FirstPersonCameraComponent->SetRelativeLocation(FVector(-39.56f, 1.75f, 64.f)); // Position the camera
	FirstPersonCameraComponent->bUsePawnControlRotation = true;

	// Create a mesh component that will be used when being viewed from a '1st person' view (when controlling this pawn)
	Mesh1P = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("CharacterMesh1P"));
	Mesh1P->SetupAttachment(GetCapsuleComponent());

	RegenStamina = true;
}

void ARLGLCharacter::BeginPlay()
{
	// Call the base class
	Super::BeginPlay();

	CurrStamina = MaxStamina;
}

void ARLGLCharacter::Tick(float DeltaSeconds)
{
	UpdateStamina(DeltaSeconds);
}

//////////////////////////////////////////////////////////////////////////
// Input

void ARLGLCharacter::SetupPlayerInputComponent(class UInputComponent *PlayerInputComponent)
{
	// set up gameplay key bindings
	check(PlayerInputComponent);

	// Bind jump events
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);

	// Bind sprint events
	PlayerInputComponent->BindAction("Run", IE_Pressed, this, &ARLGLCharacter::StartRun);
	PlayerInputComponent->BindAction("Run", IE_Released, this, &ARLGLCharacter::StopRun);

	// Bind movement events
	PlayerInputComponent->BindAxis("MoveForward", this, &ARLGLCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &ARLGLCharacter::MoveRight);

	// We have 2 versions of the rotation bindings to handle different kinds of devices differently
	// "turn" handles devices that provide an absolute delta, such as a mouse.
	// "turnrate" is for devices that we choose to treat as a rate of change, such as an analog joystick
	PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("TurnRate", this, &ARLGLCharacter::TurnAtRate);
	PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("LookUpRate", this, &ARLGLCharacter::LookUpAtRate);

	// Bind quit event
	PlayerInputComponent->BindAction("Quit", IE_Pressed, this, &ARLGLCharacter::QuitGame);
}

void ARLGLCharacter::MoveForward(float Value)
{
	if (Value != 0.0f)
	{
		// add movement in that direction
		AddMovementInput(GetActorForwardVector(), Value);
	}
}

void ARLGLCharacter::MoveRight(float Value)
{
	if (Value != 0.0f)
	{
		// add movement in that direction
		AddMovementInput(GetActorRightVector(), Value);
	}
}

void ARLGLCharacter::TurnAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}

void ARLGLCharacter::LookUpAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
}

void ARLGLCharacter::DoRagdoll()
{
	// Stop any movement
	UCharacterMovementComponent *const cMove = GetCharacterMovement();
	cMove->SetMovementMode(EMovementMode::MOVE_None);
	cMove->StopMovementImmediately();

	// Disable collision of the capsule collider
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	// Ragdoll the character mesh
	Mesh1P->SetCollisionObjectType(ECollisionChannel::ECC_PhysicsBody);
	Mesh1P->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	Mesh1P->SetSimulatePhysics(true);
}

void ARLGLCharacter::StartRun()
{
	GetCharacterMovement()->MaxWalkSpeed = MaxWalkSpeed;

	if (staminaTimer.IsValid())
		staminaTimer.Invalidate();
	RegenStamina = false;
}

void ARLGLCharacter::StopRun()
{
	GetCharacterMovement()->MaxWalkSpeed = DefaultWalkSpeed;
	GetWorldTimerManager().SetTimer(staminaTimer, this, &ARLGLCharacter::ActivateStaminaRegen, StaminaRegenDelay);
}

void ARLGLCharacter::UpdateStamina(float DeltaSeconds)
{
	if (RegenStamina)
	{
		if (CurrStamina >= MaxStamina)
		{
			CurrStamina = MaxStamina;
			return;
		}
		CurrStamina += StaminaRegenRate * DeltaSeconds;
	}
	else
	{
		if (CurrStamina <= 0.0f)
		{
			CurrStamina = 0.0f;
			GetCharacterMovement()->MaxWalkSpeed = DefaultWalkSpeed;
			return;
		}

		if (GetCharacterMovement()->Velocity.Size() > 0.0f)
			CurrStamina -= DeltaSeconds;
	}
}

void ARLGLCharacter::ActivateStaminaRegen()
{
	RegenStamina = true;
}

void ARLGLCharacter::QuitGame()
{
	UWorld *world = GetWorld();
	APlayerController *controller = world->GetFirstPlayerController();
	UKismetSystemLibrary::QuitGame(world, controller, EQuitPreference::Quit, true);
}