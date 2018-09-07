// Fill out your copyright notice in the Description page of Project Settings.

#include "EnemyGoal.h"
#include "Components/SphereComponent.h"
#include "Enemy.h"
#include "Waypoint.h"
#include "Kismet/GameplayStatics.h"
#include "TDGameMode.h"
#include "Runtime/Engine/Public/TimerManager.h"
#include "Runtime/Engine/Classes/Engine/Engine.h"

// Sets default values
AEnemyGoal::AEnemyGoal()
{
	// Use a sphere as a simple collision representation
	CollisionComp = CreateDefaultSubobject<USphereComponent>(TEXT("Sphere Component"));
	CollisionComp->InitSphereRadius(100.f);
	CollisionComp->BodyInstance.SetCollisionProfileName("Goal");
	CollisionComp->OnComponentHit.AddDynamic(this, &AEnemyGoal::OnHit);	

	RootComponent = CollisionComp;

	// No enemies yet.
	EnemyLoad = 0;

	// Default number of added enemy points to trigger GameOver condition 
	MaxEnemyLoad = 50;
}

// Called when the game starts or when spawned
void AEnemyGoal::BeginPlay()
{
	Super::BeginPlay();	
}


/* Check if overlapped object is an Enemy, if so, destroy it. */
void AEnemyGoal::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, 
					   FVector NormalImpulse, const FHitResult& Hit)
{
	//GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Blue, OtherActor->GetClass()->GetName());
	if (OtherActor->IsA(AEnemy::StaticClass())) // An enemy has reached this goal
	{
		// Increment this goal's enemy load by the enemy's resource value 
		// (this adds more weight to stronger enemies)
		EnemyLoad += Cast<AEnemy>(OtherActor)->GetResourceValue(); 
		OtherActor->Destroy(); // Enemy is dismissed

		// Play a sound when the enemy reaches the goal
		if (EnemySound != NULL)
		{
			UGameplayStatics::PlaySoundAtLocation(this, EnemySound, GetActorLocation());
		}

		ATDGameMode * TDGameMode = Cast<ATDGameMode>(UGameplayStatics::GetGameMode(this));

		// Too many enemies have reached this goal. End game.
		if ((EnemyLoad >= MaxEnemyLoad) && 
			(TDGameMode->GetTDGameState() == ETDGameState::InGame))
		{
			// Set game status as GameOver
			TDGameMode->SetTDGameState(ETDGameState::GameOver);

			FTimerHandle TimerHandle;
			FTimerDelegate TimerDelegate;
			TimerDelegate.BindUFunction(this, FName("RestartLevel"));

			// Restart level after delay
			GetWorldTimerManager().SetTimer(TimerHandle, TimerDelegate, 5.f, false);
		}
	}
}

/* For when the Game Over state is reached. */
void AEnemyGoal::RestartLevel()
{
	UGameplayStatics::OpenLevel(this, FName(*GetWorld()->GetName()), false);
}

