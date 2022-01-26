// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "RLGLGameMode.generated.h"

class APawn;
class AActor;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnResetGame);

UCLASS(minimalapi)
class ARLGLGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	ARLGLGameMode();

	// Restart game
	UFUNCTION(BlueprintCallable)
	void ReloadGame();

	// Clean up and restore the game
	UFUNCTION(BlueprintCallable)
	void ResetGame();

	// Reset delegate for observer (needed for ResetGame since that only resets the AI and the player, not the observer and thus minigame)
	UPROPERTY(BlueprintAssignable, Category = EventDispatchers)
	FOnResetGame OnResetGame;

protected:
	virtual void BeginPlay();

private:
	TArray<AActor *> spawnpoints;
	TSubclassOf<APawn> AIClass;

	void Spawn(TSubclassOf<APawn> pawnClass, FVector pos);
	void InitSpawnpoints();
};
