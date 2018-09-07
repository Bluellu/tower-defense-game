// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "DefenseTowerBase.generated.h"

UCLASS()
class TD_API ADefenseTowerBase : public AActor
{
	GENERATED_BODY()

protected:

	/** Status that indicates whether this tower is still attached to the player. */
	UPROPERTY(BlueprintReadOnly)
	bool bDeployed;
	
public:	
	// Sets default values for this actor's properties
	ADefenseTowerBase();

	/** How much it costs for the player to build this tower. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int ResourceCost;

	/** Reference to the player character it was created by. */
	UPROPERTY()
	APawn *Creator;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	/** Check if this Tower is not overlapping with other objects. */
	UFUNCTION(BlueprintCallable)
	bool IsUnobstructed();	

	/** Function to allow Player to set this Tower down */
	virtual void Deploy();

	/** Set Player Creator of this tower */
	FORCEINLINE void SetCreator(APawn *PCreator) { Creator = PCreator; }

	/** Check if tower is currently deployed (as opposed to attached). */
	UFUNCTION(BlueprintCallable)
	FORCEINLINE bool IsDeployed() { return bDeployed; }

	/** Get the cost of deployment for this tower. */
	FORCEINLINE int GetCost() { return ResourceCost; }
	
};
