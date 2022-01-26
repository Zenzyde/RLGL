// Copyright Epic Games, Inc. All Rights Reserved.

#include "RLGLGameMode.h"
#include "RLGLHUD.h"
#include "RLGLCharacter.h"
#include "UObject/ConstructorHelpers.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/Engine.h"

ARLGLGameMode::ARLGLGameMode()
	: Super()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnClassFinder(TEXT("/Game/FirstPersonCPP/Blueprints/FirstPersonCharacter"));
	DefaultPawnClass = PlayerPawnClassFinder.Class;

	// use our custom HUD class
	HUDClass = ARLGLHUD::StaticClass();

	// set default ai pawn class to our Blueprinted ai character
	static ConstructorHelpers::FClassFinder<APawn> AIClassFinder(TEXT("/Game/FirstPersonCPP/Blueprints/AI/ParticipantAICharacterBP"));
	AIClass = AIClassFinder.Class;
}

void ARLGLGameMode::BeginPlay()
{
	InitSpawnpoints();

	int playerSpawn = FMath::RandRange(0, spawnpoints.Num() - 1);
	for (size_t i = 0; i < spawnpoints.Num(); i++)
	{
		if (spawnpoints[i] != NULL)
		{
			if (i == playerSpawn)
			{
				Spawn(DefaultPawnClass, spawnpoints[i]->GetActorLocation());
			}
			else
			{
				Spawn(AIClass, spawnpoints[i]->GetActorLocation());
			}
		}
	}
}

void ARLGLGameMode::InitSpawnpoints()
{
	UWorld const *world = GetWorld();
	UGameplayStatics::GetAllActorsWithTag(world, FName("Spawnpoint"), spawnpoints);
}

void ARLGLGameMode::Spawn(TSubclassOf<APawn> pawnClass, FVector pos)
{
	UWorld *world = GetWorld();
	FActorSpawnParameters spawnParams;
	spawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::DontSpawnIfColliding;

	if (pawnClass == AIClass)
	{
		world->SpawnActor<ACharacter>(AIClass, pos, FRotator::ZeroRotator, spawnParams);
	}
	else
	{
		world->SpawnActor<ARLGLCharacter>(DefaultPawnClass, pos, FRotator::ZeroRotator, spawnParams);
	}
}

// Completely reset the game on player death
void ARLGLGameMode::ReloadGame()
{
	UWorld *world = GetWorld();
	UGameplayStatics::OpenLevel(world, FName(*world->GetName()));
}

// Restore the players position, destroy all AI pawns and respawn them, restart the observer
void ARLGLGameMode::ResetGame()
{
	UWorld *world = GetWorld();

	// Reset player position
	int posIndex = FMath::RandRange(0, spawnpoints.Num() - 1);

	ARLGLCharacter *player = Cast<ARLGLCharacter>(UGameplayStatics::GetActorOfClass(world, ARLGLCharacter::StaticClass()));

	if (player != nullptr)
		player->SetActorLocation(spawnpoints[posIndex]->GetActorLocation());

	// Destroy all AI participants
	TArray<AActor *> aiParticipants;
	UGameplayStatics::GetAllActorsOfClass(world, AIClass, aiParticipants);
	for (size_t i = 0; i < aiParticipants.Num(); i++)
	{
		aiParticipants[i]->Destroy();
	}
	// Spawn new AI participants
	for (size_t i = 0; i < spawnpoints.Num(); i++)
	{
		Spawn(AIClass, spawnpoints[i]->GetActorLocation());
	}

	// Call to reset observer and "minigame"
	OnResetGame.Broadcast();
}