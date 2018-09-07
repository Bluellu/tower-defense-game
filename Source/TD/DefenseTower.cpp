// Fill out your copyright notice in the Description page of Project Settings.

#include "DefenseTower.h"
#include "TDCharacter.h"
#include "Camera/CameraComponent.h"
#include "Runtime/Engine/Classes/GameFramework/Actor.h"
#include "TDProjectile.h"
#include "Runtime/Engine/Classes/Engine/World.h"
#include "Runtime/Engine/Classes/Kismet/KismetMathLibrary.h"
#include "Components/SphereComponent.h"
#include "Enemy.h"
#include "Animation/AnimInstance.h"
#include "Runtime/Engine/Public/TimerManager.h"
#include "Kismet/GameplayStatics.h"

#include "DrawDebugHelpers.h"
#include "Runtime/Engine/Classes/Engine/Engine.h"

// Sets default values
ADefenseTower::ADefenseTower()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	ResourceCost = 30;

	AttackPower = 10;
	AttackInterval = 1.0f; // Interval between attacks in seconds

	TargetEnemy = 0;	// No enemy targeted yet
	bCanShoot = true;	// For shooting-interval timer set up 

	// Initialize trigger sphere (for enemy targetting).
	TriggerSphere = CreateDefaultSubobject<USphereComponent>(TEXT("Sphere Trigger Component"));
	TriggerSphere->InitSphereRadius(TriggerRadius);
	TriggerSphere->SetupAttachment(RootComponent);

	// Register events
	TriggerSphere->bGenerateOverlapEvents = true;
	TriggerSphere->OnComponentBeginOverlap.AddDynamic(this, &ADefenseTower::OnOverlapBegin);
	TriggerSphere->OnComponentEndOverlap.AddDynamic(this, &ADefenseTower::OnOverlapEnd);

	TriggerSphere->SetCollisionProfileName("NoCollision"); // We don't need this before deployment.

	// GUN ANIMATION SETUP 
	MeshDT = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("CharacterMesh1P"));
	MeshDT->SetupAttachment(RootComponent);
	MeshDT->bCastDynamicShadow = false;
	MeshDT->CastShadow = false;

	// Create a gun mesh component
	GunComponent = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("GunComponent"));
	GunComponent->bCastDynamicShadow = false;
	GunComponent->CastShadow = false;
	GunComponent->SetupAttachment(RootComponent);

}


// Called when the game starts or when spawned
void ADefenseTower::BeginPlay()
{
	Super::BeginPlay();	

	MeshDT->AttachToComponent(TriggerSphere, FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true), TEXT("ArmsAttachment"));
	MeshDT->RelativeLocation = FVector(0.0f, 0.0f, -130.0f);
	MeshDT->RelativeScale3D = FVector(1.5f, 1.5f, 1.5f);

	//Attach gun mesh component to Skeleton, doing it here because the skeleton is not yet created in the constructor
	GunComponent->AttachToComponent(MeshDT, FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true), TEXT("GripPoint"));
}


// Called every frame
void ADefenseTower::Tick(float DeltaTime)
{
	//DrawDebugSphere(GetWorld(), GetActorLocation(), TriggerRadius, 50, FColor::Green, false, (0.2f), 0, 2);
	Super::Tick(DeltaTime);
	if (!bDeployed)
	{
		if (Creator)
		{
			ATDCharacter *Player = Cast<ATDCharacter>(Creator);
			FVector ForwardVector = Player->GetFirstPersonCameraComponent()->GetForwardVector();
			float Offset = 300.f;
			FVector Destination = (Player->GetActorLocation() + (ForwardVector * Offset)); 
			Destination.Z = 180.f;
			this->SetActorLocation(Destination);
		}
	}
	else // Deployed
	{
		// Try to shoot target (if exists)
		if (TargetEnemy)
		{
			if (TargetEnemy->IsPendingKill()) // Target is being destroyed elsewhere: reset.
			{
				TargetEnemy = 0;
				PrimaryActorTick.bCanEverTick = false; // Disable ticking to improve performance
			}

			if (TargetEnemy) // Target is valid: shoot.
			{
				// Keep rotating gun towards current target (On Z axis only)
				FVector GunLoc = MeshDT->GetComponentLocation();
				FVector TargetLoc = TargetEnemy->GetActorLocation();
				FVector Direction = (TargetLoc - GunLoc);
				Direction.Normalize();
				FRotator ZRotation = FRotator(0.0f, Direction.Rotation().Yaw, 0.0f);  

				MeshDT->SetWorldRotation(FMath::Lerp(MeshDT->GetComponentRotation(), ZRotation, 0.5f));

				if (bCanShoot)
				{
					ShootTarget(TargetEnemy);
					bCanShoot = false; 
					PrimaryActorTick.bCanEverTick = false;

					// Set timer until next shot is allowed.
					FTimerHandle TimerHandle;
					FTimerDelegate TimerDelegate;
					TimerDelegate.BindUFunction(this, FName("AllowShooting"));

					// Spawn new Enemy after a small delay
					GetWorldTimerManager().SetTimer(TimerHandle, TimerDelegate, AttackInterval, false);
				}
			}
		}
	}
}


/* Check if an enemy has entered the trigger, and set it as target if none exists */
void ADefenseTower::OnOverlapBegin(class UPrimitiveComponent* HitComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult)
{
	//GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Red, OtherActor->GetClass()->GetName());
	if (!TargetEnemy)
	{
		TargetEnemy = Cast<APawn>(OtherActor);
		PrimaryActorTick.bCanEverTick = true; // Enable tick for shooting logic 
	}
}


/* Check if a currently targeted enemy has left the trigger zone */
void ADefenseTower::OnOverlapEnd(class UPrimitiveComponent* HitComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (TargetEnemy)
	{
		if (OtherActor->GetName() == TargetEnemy->GetName()) // Target has left trigger, reset 
		{
			TargetEnemy = 0;
			PrimaryActorTick.bCanEverTick = false;
		}
	}
}


/* Shoot a projectile at a target. */
void ADefenseTower::ShootTarget(APawn *Target)
{
	FVector Start = this->GetActorLocation() + FVector(0.0f, 0.0f, 100.f); // Tower position
	FVector End = Target->GetActorLocation(); // Target position
	FVector ForwardVector = End - Start; // Use to set up animation

	//DrawDebugLine(GetWorld(), Start, End, FColor::Blue, false, 0.5f);
	if (Target->IsA(AEnemy::StaticClass())) 
	{
		AEnemy *TargetEnem = Cast<AEnemy>(Target);
		ATDCharacter *Player = Cast<ATDCharacter>(Creator);

		// try and play the sound if specified
		if (FireSound != NULL)
		{
			UGameplayStatics::PlaySoundAtLocation(this, FireSound, GetActorLocation());
		}

		// try and play a firing animation if specified
		if (FireAnimation != NULL)
		{
			// Get the animation object for the arms mesh
			UAnimInstance* AnimInstance = MeshDT->GetAnimInstance();
			if (AnimInstance != NULL)
			{
				AnimInstance->Montage_Play(FireAnimation, 1.f);
			}
		}

		// Inflict damage and give resource points to the player (0 if enemy still alive).
		int ResourceGiven = TargetEnem->ReceiveDamage(AttackPower);
		Player->IncrementResourcePts(ResourceGiven);

		// If Resource Points given are more than 0, then the enemy is dead. Remove target.
		if (ResourceGiven > 0)
		{
			Target = 0;
			PrimaryActorTick.bCanEverTick = false;			
		}
	}
}

/* Function to allow Player to set this Tower down */
void ADefenseTower::Deploy() { 
	bDeployed = true; 
	TriggerSphere->SetCollisionProfileName("OnlyTriggerEnemy"); 
}


/* Allow tower to shoot target (handler for shooting interval timer). */
void ADefenseTower::AllowShooting()
{
	bCanShoot = true;
	PrimaryActorTick.bCanEverTick = true;
}