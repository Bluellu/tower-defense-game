// Fill out your copyright notice in the Description page of Project Settings.

#include "StickyTower.h"
#include "Components/BoxComponent.h"
#include "TDCharacter.h"
#include "Camera/CameraComponent.h"
#include "Runtime/Engine/Classes/GameFramework/Actor.h"
#include "Runtime/Engine/Classes/GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"

#include "Runtime/Engine/Classes/Engine/Engine.h"


// Sets default values
AStickyTower::AStickyTower()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	ResourceCost = 20;

	SlowingRate = 10.f;
}

// Called when the game starts or when spawned
void AStickyTower::BeginPlay()
{
	Super::BeginPlay();
	UBoxComponent *BoxCollider = GetCollisionBox();
	BoxCollider->OnComponentBeginOverlap.AddDynamic(this, &AStickyTower::OnOverlapBegin);
	BoxCollider->OnComponentEndOverlap.AddDynamic(this, &AStickyTower::OnOverlapEnd);
	
}

// Called every frame
void AStickyTower::Tick(float DeltaTime)
{
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
		// For this tower, we don't need to tick anymore.
		PrimaryActorTick.bCanEverTick = false;
	}
}

/* Function to allow Player to set this Tower down */
void AStickyTower::Deploy()
{
	bDeployed = true;
	GetCollisionBox()->SetCollisionProfileName("OnlyTriggerEnemy");
}


/* Slow enemies that touch this tower down by the slowing rate. */
void AStickyTower::OnOverlapBegin(class UPrimitiveComponent* HitComp, class AActor* OtherActor, 
								  class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
								  bool bFromSweep, const FHitResult & SweepResult)
{
	if (bDeployed) 
	{
		ACharacter *Enemy = Cast<ACharacter>(OtherActor);
		UCharacterMovementComponent *EnemyMovementComp = Enemy->GetCharacterMovement();

		// Lower this enemy's speed.
		EnemyMovementComp->MaxWalkSpeed = (EnemyMovementComp->MaxWalkSpeed) / SlowingRate;
	}
}

/* Return enemies to their regular speed at the end of overlap. */
void AStickyTower::OnOverlapEnd(class UPrimitiveComponent* HitComp, class AActor* OtherActor,
								class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (bDeployed)
	{
		ACharacter *Enemy = Cast<ACharacter>(OtherActor);
		UCharacterMovementComponent *EnemyMovementComp = Enemy->GetCharacterMovement();

		// Return this enemy's speed to normal.
		EnemyMovementComp->MaxWalkSpeed = (EnemyMovementComp->MaxWalkSpeed) * SlowingRate;
	}
}
