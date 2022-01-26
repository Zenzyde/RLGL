// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "RLGLCharacter.generated.h"

class UInputComponent;
class USkeletalMeshComponent;
class USceneComponent;
class UCameraComponent;
class UAnimMontage;
class USoundBase;
class USpringArmComponent;

UCLASS(config = Game)
class ARLGLCharacter : public ACharacter
{
	GENERATED_BODY()

	/** Pawn mesh: 1st person view (arms; seen only by self) */
	UPROPERTY(VisibleDefaultsOnly, Category = Mesh)
	USkeletalMeshComponent *Mesh1P;

	/** First person camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	UCameraComponent *FirstPersonCameraComponent;

	// Camera boom for ragdoll mode
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	USpringArmComponent *camBoom;

	// Enable stamina to regenerate
	bool RegenStamina;

	// Stamina regen timer handle
	FTimerHandle staminaTimer;

public:
	ARLGLCharacter();

protected:
	virtual void BeginPlay();

	virtual void Tick(float DeltaSeconds);

	// APawn interface
	virtual void SetupPlayerInputComponent(UInputComponent *InputComponent) override;
	// End of APawn interface

public:
	/** Base turn rate, in deg/sec. Other scaling may affect final turn rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
	float BaseTurnRate;

	/** Base look up/down rate, in deg/sec. Other scaling may affect final rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
	float BaseLookUpRate;

	// Max walk speed
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Movement)
	float MaxWalkSpeed;

	// Default walk speed
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Movement)
	float DefaultWalkSpeed;

	// Max stamina
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Movement)
	float MaxStamina;

	// Current stamina
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Movement)
	float CurrStamina;

	// Stamina regen rate
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Movement)
	float StaminaRegenRate;

	// Stamina regen delay
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Movement)
	float StaminaRegenDelay;

	// Amount of games survived this session
	UPROPERTY(BlueprintReadWrite, Category = Gameplay)
	int roundsSurvived;

protected:
	/** Handles moving forward/backward */
	void MoveForward(float Val);

	/** Handles stafing movement, left and right */
	void MoveRight(float Val);

	/**
	 * Called via input to turn at a given rate.
	 * @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	 */
	void TurnAtRate(float Rate);

	/**
	 * Called via input to turn look up/down at a given rate.
	 * @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	 */
	void LookUpAtRate(float Rate);

	// Enables sprinting
	void StartRun();
	// Disables sprinting
	void StopRun();

	// Manages stamina usage/regen
	void UpdateStamina(float DeltaSeconds);

	// Handles delayed stamina regen activation
	void ActivateStaminaRegen();

	// Exit game
	void QuitGame();

public:
	/** Returns Mesh1P subobject **/
	USkeletalMeshComponent *GetMesh1P() const { return Mesh1P; }
	/** Returns FirstPersonCameraComponent subobject **/
	UCameraComponent *GetFirstPersonCameraComponent() const { return FirstPersonCameraComponent; }

	// Activate ragdolling & deactivate 1st person mesh
	UFUNCTION(BlueprintCallable)
	void DoRagdoll();
};
