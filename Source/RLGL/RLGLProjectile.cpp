// Copyright Epic Games, Inc. All Rights Reserved.

#include "RLGLProjectile.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Components/SphereComponent.h"
#include "RLGLCharacter.h"
#include "RLGLGameMode.h"
#include "Kismet/GameplayStatics.h"

ARLGLProjectile::ARLGLProjectile()
{
	// Create projectile mesh/collision
	ProjectileMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ProjectileMesh"));
	ProjectileMesh->OnComponentHit.AddDynamic(this, &ARLGLProjectile::OnHit);

	// Use a sphere as a simple collision representation
	// CollisionComp = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComp"));
	// CollisionComp->InitSphereRadius(5.0f);
	// CollisionComp->BodyInstance.SetCollisionProfileName("Projectile");
	// CollisionComp->OnComponentHit.AddDynamic(this, &ARLGLProjectile::OnHit); // set up a notification for when this component hits something blocking

	ProjectileMesh->SetWalkableSlopeOverride(FWalkableSlopeOverride(WalkableSlope_Unwalkable, 0.f));
	ProjectileMesh->CanCharacterStepUpOn = ECB_No;

	// Players can't walk on it
	// CollisionComp->SetWalkableSlopeOverride(FWalkableSlopeOverride(WalkableSlope_Unwalkable, 0.f));
	// CollisionComp->CanCharacterStepUpOn = ECB_No;

	// Set as root component
	RootComponent = ProjectileMesh; // CollisionComp;

	// Use a ProjectileMovementComponent to govern this projectile's movement
	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileComp"));
	ProjectileMovement->UpdatedComponent = ProjectileMesh; // CollisionComp;
	ProjectileMovement->InitialSpeed = 3000.f;
	ProjectileMovement->MaxSpeed = 3000.f;
	ProjectileMovement->bRotationFollowsVelocity = true;
	ProjectileMovement->bShouldBounce = false;
}

void ARLGLProjectile::OnHit(UPrimitiveComponent *HitComp, AActor *OtherActor, UPrimitiveComponent *OtherComp, FVector NormalImpulse, const FHitResult &Hit)
{
	// Only add impulse and destroy projectile if we hit a physics
	if ((OtherActor != nullptr) && (OtherActor != this) && (OtherComp != nullptr) && OtherComp->IsSimulatingPhysics())
	{
		ARLGLCharacter *player = Cast<ARLGLCharacter>(OtherActor);
		if (player != nullptr)
		{
			UWorld *world = GetWorld();
			ARLGLGameMode *gamemode = Cast<ARLGLGameMode>(world->GetAuthGameMode());
			if (gamemode != nullptr)
			{
				gamemode->ReloadGame();
			}
		}
		else // Most likely hit an AI or something
		{
			if (BloodSplatterVFX != nullptr)
			{
				UWorld *world = GetWorld();
				// Spawn bloodsplatter
				UGameplayStatics::SpawnEmitterAtLocation(world, BloodSplatterVFX, Hit.ImpactPoint);
				// Let the blueprint handle ragdolling the AI
			}
		}
	}
	else if ((OtherActor != nullptr) && (OtherActor != this) && (OtherComp != nullptr) && OtherActor->IsRootComponentStatic())
	{
		Destroy();
	}
}