// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#include "TDGameMode.h"
#include "TDHUD.h"
#include "TDCharacter.h"
#include "UObject/ConstructorHelpers.h"
#include "Kismet/GameplayStatics.h"
#include "Runtime/Engine/Classes/Engine/Engine.h"

ATDGameMode::ATDGameMode()
	: Super()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnClassFinder(TEXT("/Game/FirstPersonCPP/Blueprints/FirstPersonCharacter"));
	DefaultPawnClass = PlayerPawnClassFinder.Class;

	// use our custom HUD class
	HUDClass = ATDHUD::StaticClass();

	// Set initial Game State
	TDGameState = ETDGameState::InGame;
}

