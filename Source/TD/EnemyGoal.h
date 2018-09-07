// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "EnemyGoal.generated.h"


UCLASS()
class TD_API AEnemyGoal : public AActor
{
	GENERATED_BODY()

	/* Sphere collision component */
	UPROPERTY(VisibleDefaultsOnly, Category = Goal)
	class USphereComponent* CollisionComp;
	
public:	
	// Sets default values for this actor's properties
	AEnemyGoal();

	/* The number of enemies that have reached this goal. */
	UPROPERTY(BlueprintReadOnly, Category = Goal)
	int EnemyLoad;
	
	/* The number of arriving enemies that trigger losing condition. */
	UPROPERTY(EditAnywhere, Category = Goal)
	int MaxEnemyLoad;

	/** Sound to play each time an enemy reaches this goal */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
	class USoundBase* EnemySound;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	/** Function to handle actors that collide with this goal. */
	UFUNCTION()
	void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	/** To be called upon game over callback. */
	UFUNCTION()
	void RestartLevel();

};
