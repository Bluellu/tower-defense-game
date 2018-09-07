// Fill out your copyright notice in the Description page of Project Settings.

#include "EnemySpawner.h"
#include "Engine/World.h"
#include "Enemy.h"
#include "EnemyAIController.h"
#include "Runtime/CoreUObject/Public/UObject/ConstructorHelpers.h"
#include "Runtime/Engine/Public/TimerManager.h"
#include "TDGameMode.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"
#include "Runtime/Engine/Classes/GameFramework/PlayerController.h"
#include "TDHUD.h"


// Sets default values
AEnemySpawner::AEnemySpawner()
{
}

// Called when the game starts or when spawned
void AEnemySpawner::BeginPlay()
{
	Super::BeginPlay();	
}

/* Spawns a single Enemy of a given type */
void AEnemySpawner::SpawnEnemy(TSubclassOf<AEnemy> EnemyClass)
{
	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = this;
	FVector SpawnLocation = this->GetActorLocation();
	FRotator Rotator = FRotator(0);
		
	// Spawn Enemy at this Spawner's location           
	UWorld* const World = GetWorld();
	if (World != NULL)
	{
		APawn *SpawnedEnemy = GetWorld()->SpawnActor<AEnemy>(EnemyClass, 
															 SpawnLocation, 
															 Rotator, 
															 SpawnParams);

		if (SpawnedEnemy)
		{
			// Set and spawn this enemy's default controller.
			SpawnedEnemy->AIControllerClass = AEnemyAIController::StaticClass();
			SpawnedEnemy->SpawnDefaultController();
		}
	}	
} 


/* Spawn a custom wave of Enemies of a chosen type (for Blueprint use). */
void AEnemySpawner::SpawnWave(TSubclassOf<AEnemy> EnemyClass, int NumEnemies)
{
	float SpawnDelay = 0.5f; // The initial delay

	for (int i = 0; i < NumEnemies; i++)
	{
		FTimerHandle TimerHandle;
		FTimerDelegate TimerDelegate;
		TimerDelegate.BindUFunction(this, FName("SpawnEnemy"), EnemyClass);

		// Spawn new Enemy after a small delay
		GetWorldTimerManager().SetTimer(TimerHandle, TimerDelegate, SpawnDelay, false);
		SpawnDelay += 0.5f; 
	}
}

/* Call once all waves have gone by without defeat being triggered.
For blueprint use.
*/
void AEnemySpawner::CallVictory()
{
	ATDGameMode * TDGameMode = Cast<ATDGameMode>(UGameplayStatics::GetGameMode(this));

	// If Game Over has not already occurred, set game status as Victory
	if (TDGameMode->GetTDGameState() == ETDGameState::InGame)
		TDGameMode->SetTDGameState(ETDGameState::Victory);
}

void AEnemySpawner::StartCountdown(int seconds) 
{
	APlayerController *PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	ATDHUD *HUD = Cast<ATDHUD>(PlayerController->GetHUD());
	HUD->SetCountdown(seconds);
}

/* To call after a delay post-victory (in blueprints). */
void AEnemySpawner::RestartLevel()
{
	UGameplayStatics::OpenLevel(this, FName(*GetWorld()->GetName()), false);
}

