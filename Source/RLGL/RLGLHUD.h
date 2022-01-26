// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once 

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "RLGLHUD.generated.h"

UCLASS()
class ARLGLHUD : public AHUD
{
	GENERATED_BODY()

public:
	ARLGLHUD();

	/** Primary draw call for the HUD */
	virtual void DrawHUD() override;

private:
	/** Crosshair asset pointer */
	class UTexture2D* CrosshairTex;

};

