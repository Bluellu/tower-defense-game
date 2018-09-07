// Fill out your copyright notice in the Description page of Project Settings.

#include "DefenseTowerBase.h"
#include "Runtime/Engine/Classes/GameFramework/Actor.h"


// Sets default values
ADefenseTowerBase::ADefenseTowerBase()
{
 	// Set this actor to call Tick() every frame.  
	PrimaryActorTick.bCanEverTick = true;

	Creator = 0;
	bDeployed = false;
}

// Called when the game starts or when spawned
void ADefenseTowerBase::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ADefenseTowerBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

/* Check if this tower is overlapping with any other objects. */
bool ADefenseTowerBase::IsUnobstructed()
{
	TArray<AActor*> OverlappingActors;
	GetOverlappingActors(OverlappingActors);

	if (OverlappingActors.GetData())
		return false;
	return true;
}

/* Function to allow Player to set this Tower down */
void ADefenseTowerBase::Deploy()
{
	bDeployed = true;
}


