// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Enemy.generated.h"

UCLASS()
class TD_API AEnemy : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	AEnemy();
	
	/** This Enemy's Hit Points */
	UPROPERTY(EditAnywhere, Category = HP)
	int HP;

	/** The amount of resource points this enemy returns on death */
	UPROPERTY(EditAnywhere, Category = HP)
	int ResourcePoints;

	/** The Behavior Tree Object */
	UPROPERTY(EditAnywhere, Category = Behavior)
	class UBehaviorTree *EnemyBehavior;

	/** This Enemy's goal */
	AActor *Goal;

	/** The half angle of the field of movement for this enemy, in degrees. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Behavior)
	float ConeHalfAngle;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	/** Subtract damage from HP */
	int ReceiveDamage(int Damage);

	/** Handler for receiving damage */
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, meta = (DisplayName = "OnTakingDamage"))
	void OnTakingDamage();	

	/** GetBlueprint-defined sensing sphere, so that it can be accessed in the behavior tree checks. */
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, meta = (DisplayName = "GetSensingSphere"))
	USphereComponent *GetSensingSphere();

	FORCEINLINE AActor *GetGoal() { return Goal; }

	FORCEINLINE float GetConeHalfAngle() { return ConeHalfAngle; }

	FORCEINLINE int GetResourceValue() { return ResourcePoints; }

};
