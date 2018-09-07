// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "EnemySpawner.generated.h"

UCLASS()
class TD_API AEnemySpawner : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AEnemySpawner();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	/** Spawn a single Enemy of a given type. */
	UFUNCTION(BlueprintCallable)
	void SpawnEnemy(TSubclassOf<AEnemy> EnemyClass);

	/** Start a custom wave of Enemies of a chosen type. */
	UFUNCTION(BlueprintCallable, Category = Spawner)
	void SpawnWave(TSubclassOf<AEnemy> EnemyClass, int NumEnemies);

	/** Display a countdown on the HUD between enemy waves. */
	UFUNCTION(BlueprintCallable, Category = Spawner)
	void StartCountdown(int seconds);

	/** Restart this level (To call after victory). */
	UFUNCTION(BlueprintCallable)
	void RestartLevel();

	/** Set the game status to Victory (to call after final wave). */
	UFUNCTION(BlueprintCallable)
	void CallVictory();
};
