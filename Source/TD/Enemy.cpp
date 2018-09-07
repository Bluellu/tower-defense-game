// Fill out your copyright notice in the Description page of Project Settings.

#include "Enemy.h"
#include "Components/SphereComponent.h"
#include "Runtime/Engine/Classes/Engine/Engine.h"
#include "Kismet/GameplayStatics.h"
#include "EnemyGoal.h"
#include "Components/ArrowComponent.h"

// Sets default values
AEnemy::AEnemy()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	// Set default values
	HP = 10;
	ResourcePoints = 10;
	Goal = 0;
	ConeHalfAngle = 90.f; // Default range of movement: 180 degrees.
}

// Called when the game starts or when spawned
void AEnemy::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void AEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);	
}

// Called to bind functionality to input
void AEnemy::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

/* Receive the given amount of damage and self-destruct if HP is depleted.
   Return ResourcePoints for player usage. */
int AEnemy::ReceiveDamage(int Damage)
{
	OnTakingDamage(); // For Blueprint reaction (material changes)

	if (HP != 0)
	{
		HP -= Damage;
		if (HP <= 0) 
		{
			this->Destroy(); // Enemy dies
			return ResourcePoints; // Player receives resource points
		}
		else
		{
			return 0; // No resource points given
		}
	}
	else // In case Enemy HP is initially set to 0
	{
		this->Destroy();
		return ResourcePoints;
	}
}
