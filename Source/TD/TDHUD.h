// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#pragma once 

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "TDHUD.generated.h"

UCLASS()
class ATDHUD : public AHUD
{
	GENERATED_BODY()

	UPROPERTY()
	UFont* HUDFont;

	/** Seconds for wave countdown display */
	int CountdownSeconds;

public:
	ATDHUD();

	/** Primary draw call for the HUD */
	virtual void DrawHUD() override;

	void DisplayResourceWarning();

	/** To start a countdown display for enemy waves */
	UFUNCTION(BlueprintCallable)
	void SetCountdown(int seconds);

private:
	/** Crosshair asset pointer */
	class UTexture2D* CrosshairTex;
};

