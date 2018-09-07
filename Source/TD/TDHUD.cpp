// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#include "TDHUD.h"
#include "Engine/Canvas.h"
#include "Engine/Texture2D.h"
#include "TextureResource.h"
#include "CanvasItem.h"
#include "UObject/ConstructorHelpers.h"
#include "Kismet/GameplayStatics.h"
#include "TDCharacter.h"
#include "TDGameMode.h"
#include "Runtime/Engine/Public/TimerManager.h"

#include "Runtime/Engine/Classes/Engine/Engine.h"


ATDHUD::ATDHUD()
{
	// Set the crosshair texture
	static ConstructorHelpers::FObjectFinder<UTexture2D> CrosshairTexObj(TEXT("/Game/FirstPerson/Textures/FirstPersonCrosshair"));
	CrosshairTex = CrosshairTexObj.Object;

	// Set the default Font 
	static ConstructorHelpers::FObjectFinder<UFont>HUDFontObj(TEXT("/Engine/EngineFonts/RobotoDistanceField"));
	HUDFont = HUDFontObj.Object;
}

void ATDHUD::DrawHUD()
{
	Super::DrawHUD();

	// Get Screen dimensions
	FVector2D ScreenDimension = FVector2D(Canvas->SizeX, Canvas->SizeY);

	// Draw very simple crosshair

	// find center of the Canvas
	const FVector2D Center(Canvas->ClipX * 0.5f, Canvas->ClipY * 0.5f);

	// offset by half the texture's dimensions so that the center of the texture aligns with the center of the Canvas
	const FVector2D CrosshairDrawPosition( (Center.X),
										   (Center.Y + 20.0f));

	// draw the crosshair
	FCanvasTileItem TileItem( CrosshairDrawPosition, CrosshairTex->Resource, FLinearColor::White);
	TileItem.BlendMode = SE_BLEND_Translucent;
	Canvas->DrawItem( TileItem );

	// Draw player resource points on the screen

	// Get Player's ResourcePoints and display them on the screen 
	ATDCharacter *Player = Cast<ATDCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
	FString ResourceDisplay = FString::Printf(TEXT("Resource Points: %d"), Player->GetResourcePts());
	DrawText(ResourceDisplay, FColor::White, 50, ScreenDimension.Y - 50, HUDFont);

	if (Cast<ATDCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0))->IsWarningEventOn())
		DisplayResourceWarning();

	// Display Countdown if applicable
	if (CountdownSeconds >= 0)
		DrawText(FString::Printf(TEXT("Enemy wave in %d"), CountdownSeconds), 
				 FColor::White, Center.X - 120, Center.Y - 160, HUDFont, 1.5f);

	// Display resource costs
	if (Player->GetPlayerState() == EPlayerMode::BuildMode)
	{
		switch (Player->GetSelectedTower())
		{
		case (ESelectedTower::Sticky):
			DrawText("STICKY PAD: 20 points", FColor::White, Center.X - 115, 50, HUDFont);
			break;
		case (ESelectedTower::Turret):
			DrawText("TURRET: 30 points", FColor::White, Center.X - 115, 50, HUDFont);
		}
	}

	// Game Over message 
	ATDGameMode *GameMode = Cast<ATDGameMode>(UGameplayStatics::GetGameMode(this));
	if (GameMode->GetTDGameState() == ETDGameState::GameOver)
		DrawText(FString("Too many enemies got through.....You lose!"), 
				 FColor::White, Center.X - 550, Center.Y, HUDFont, 2.5f);

	// Victory Message
	if (GameMode->GetTDGameState() == ETDGameState::Victory)
		DrawText(FString("You defended your base well. Congratulations!"), 
				 FColor::White, Center.X - 600, Center.Y, HUDFont, 2.5f);
}

void ATDHUD::DisplayResourceWarning()
{
	// find center of the Canvas
	const FVector2D Center(Canvas->ClipX * 0.5f, Canvas->ClipY * 0.5f);
	const FVector2D WarningDrawPosition((Center.X), (Center.Y + 20.0f));

	FString WarningDisplay = FString::Printf(TEXT("Not enough resource points!"));
	DrawText(WarningDisplay, FColor::White, Center.X - 115.f, Center.Y - 60.f , HUDFont);

}

/* Set countdown display for enemy waves */
void ATDHUD::SetCountdown(int seconds)
{
	CountdownSeconds = seconds;

	// Countdown recursively
	if (seconds >= 0)
	{
		FTimerHandle TimerHandle;
		FTimerDelegate TimerDelegate;
		TimerDelegate.BindUFunction(this, FName("SetCountdown"), seconds - 1);

		// Spawn new Enemy after a small delay
		GetWorldTimerManager().SetTimer(TimerHandle, TimerDelegate, 1.f, false);
	}
}


