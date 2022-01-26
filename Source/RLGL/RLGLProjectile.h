// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "RLGLProjectile.generated.h"

class USphereComponent;
class UProjectileMovementComponent;
class UStaticMeshComponent;

UCLASS(config = Game)
class ARLGLProjectile : public AActor
{
	GENERATED_BODY()

	/** Projectile movement component */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"))
	UProjectileMovementComponent *ProjectileMovement;

	// Projectile mesh component (also used for collision)
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent *ProjectileMesh;

public:
	ARLGLProjectile();

	// Bloodsplatter particle system
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	UParticleSystem *BloodSplatterVFX;

	/** called when projectile hits something */
	UFUNCTION()
	void OnHit(UPrimitiveComponent *HitComp, AActor *OtherActor, UPrimitiveComponent *OtherComp, FVector NormalImpulse, const FHitResult &Hit);

	// /** Returns CollisionComp subobject **/
	// USphereComponent *GetCollisionComp() const { return CollisionComp; }
	/** Returns ProjectileMovement subobject **/
	UProjectileMovementComponent *GetProjectileMovement() const { return ProjectileMovement; }
};
