// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DefenseTowerBase.h"
#include "DefenseTower.generated.h"

UCLASS()
class TD_API ADefenseTower : public ADefenseTowerBase
{
	GENERATED_BODY()

	/** Does a defense tower need arms? Yes, yes it does. */
	UPROPERTY(VisibleDefaultsOnly, Category = Mesh)
	class USkeletalMeshComponent* MeshDT;

	/** Gun mesh for shooting animation */
	UPROPERTY(VisibleDefaultsOnly, Category = Mesh)
	class USkeletalMeshComponent* GunComponent;

protected:

	/** Status that indicates if this tower is free to shoot */
	UPROPERTY(BlueprintReadOnly)
	bool bCanShoot;
	
public:	
	// Sets default values for this actor's properties
	ADefenseTower();

	/** How much damage this tower inflicts on attack.  */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int AttackPower;

	/** How long it takes before this tower can attack again. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float AttackInterval;

	/** A sphere trigger for enemy targetting. */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Trigger)
	class USphereComponent *TriggerSphere;

	/** The radius for enemy targetting. */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Trigger)
	float TriggerRadius;


	// GUN PROPERTIES

	/** Sound to play each time we fire. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
	class USoundBase* FireSound;

	/** AnimMontage to play each time we fire. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
	class UAnimMontage* FireAnimation;

	/** This tower's target Enemy (if set). */
	UPROPERTY(transient, VisibleAnywhere)
	APawn *TargetEnemy;


protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Overlap begin function
	UFUNCTION()
	void OnOverlapBegin(class UPrimitiveComponent* HitComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult);

	// Overlap end function
	UFUNCTION()
	void OnOverlapEnd(class UPrimitiveComponent* HitComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	/** Shoot a target enemy once. */
	UFUNCTION(BlueprintCallable)
	void ShootTarget(APawn *Target);

	/** Set shooting status as allowed (for handling timer intervals). */
	UFUNCTION()
	void AllowShooting();

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	/** Function to allow Player to set this Tower down */
	void Deploy() override;

};
