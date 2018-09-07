// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "TDGameMode.generated.h"

UENUM()
enum class ETDGameState : uint8
{
	InGame,
	GameOver,
	Victory
};

UCLASS(minimalapi)
class ATDGameMode : public AGameModeBase
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere)
	ETDGameState TDGameState;

public:
	ATDGameMode();

	FORCEINLINE void SetTDGameState(ETDGameState GS) { TDGameState = GS; }
	FORCEINLINE ETDGameState GetTDGameState() { return TDGameState; }
};



