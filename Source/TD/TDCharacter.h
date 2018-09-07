// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "TDCharacter.generated.h"

class UInputComponent;

/* To define the player's current mode */
UENUM()
enum class EPlayerMode : uint8
{
	AttackMode,  // Player can shoot
	BuildMode    // Player can build defense towers
};


/* For defense tower selection while in Build mode */
UENUM()
enum class ESelectedTower : uint8
{
	Turret,	    // Shoots enemies
	Sticky      // Slows enemies down
};


UCLASS(config=Game)
class ATDCharacter : public ACharacter
{
	GENERATED_BODY()

	/** Pawn mesh: 1st person view (arms; seen only by self) */
	UPROPERTY(VisibleDefaultsOnly, Category=Mesh)
	class USkeletalMeshComponent* Mesh1P;

	/** Gun mesh: 1st person view (seen only by self) */
	UPROPERTY(VisibleDefaultsOnly, Category = Mesh)
	class USkeletalMeshComponent* FP_Gun;

	/** Location on gun mesh where projectiles should spawn. */
	UPROPERTY(VisibleDefaultsOnly, Category = Mesh)
	class USceneComponent* FP_MuzzleLocation;

	/** Gun mesh: VR view (attached to the VR controller directly, no arm, just the actual gun) */
	UPROPERTY(VisibleDefaultsOnly, Category = Mesh)
	class USkeletalMeshComponent* VR_Gun;

	/** Location on VR gun mesh where projectiles should spawn. */
	UPROPERTY(VisibleDefaultsOnly, Category = Mesh)
	class USceneComponent* VR_MuzzleLocation;

	/** First person camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FirstPersonCameraComponent;

	/** Motion controller (right hand) */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class UMotionControllerComponent* R_MotionController;

	/** Motion controller (left hand) */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class UMotionControllerComponent* L_MotionController;

	/** Indicates the player's current mode (attacking or building towers). */
	UPROPERTY(VisibleAnywhere)
	EPlayerMode CurrentMode;

	/** Indicates the type of tower the player can currently spawn. */
	UPROPERTY(VisibleAnywhere)
	ESelectedTower SelectedTower;

	/** Pointer to currently held DefenseTower (if in BuildMode). */
	UPROPERTY(transient)
	AActor *AttachedTower;

	/* To hold the name of the Turret BP class we want to spawn. */
	UPROPERTY(VisibleAnywhere)
	TSubclassOf<class ADefenseTower> TurretBPClass;

	/* To hold the name of the Sticky Tower BP class we want to spawn. */
	UPROPERTY(VisibleAnywhere)
	TSubclassOf<class AStickyTower> StickyBPClass;

	/* For Triggering Screen warning event. */
	UPROPERTY()
	bool WarningEventOn;

public:
	ATDCharacter();

protected:
	virtual void BeginPlay();

public:
	/** Base turn rate, in deg/sec. Other scaling may affect final turn rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Camera)
	float BaseTurnRate;

	/** Base look up/down rate, in deg/sec. Other scaling may affect final rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Camera)
	float BaseLookUpRate;

	/** Gun muzzle's offset from the characters location */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
	FVector GunOffset;

	/** Sound to play each time we fire */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
	class USoundBase* FireSound;

	/** AnimMontage to play each time we fire */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
	class UAnimMontage* FireAnimation;

	/** Whether to use motion controller location for aiming. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
	uint32 bUsingMotionControllers : 1;

	/** How much HP a gun shot takes from Enemies. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay) 
	int GunStrength;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
	float GunRangeMultiplier;

	/** Resource Point storage. For Tower Defense Construction. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Gameplay)
	int ResourcePoints;

protected:
	
	/** Trigger handler. */
	void OnFire();

	/** Fire a projectile. */
	void AttackModeFire();

	/** Set a Defense Tower down. */
	void BuildModeFire();

	/** Set PlayerMode to BuildMode. */
	void EnterBuildMode();

	/** Return PlayerMode to AttackMode. */
	void LeaveBuildMode();

	/** Switch to next available tower. */
	void SwitchTower();

	/** Spawn DefenseTower and attach it to this Player */
	void SpawnDefenseTower();

	/** Starts Warning Event timer */
	UFUNCTION()
	void OnWarningEvent();

	/** Handles end of warning event */
	UFUNCTION()
	void EndWarningEvent();

	/** Resets HMD orientation and position in VR. */
	void OnResetVR();

	/** Handles moving forward/backward */
	void MoveForward(float Val);

	/** Handles stafing movement, left and right */
	void MoveRight(float Val);

	/**
	 * Called via input to turn at a given rate.
	 * @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	 */
	void TurnAtRate(float Rate);

	/**
	 * Called via input to turn look up/down at a given rate.
	 * @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	 */
	void LookUpAtRate(float Rate);

	struct TouchData
	{
		TouchData() { bIsPressed = false;Location=FVector::ZeroVector;}
		bool bIsPressed;
		ETouchIndex::Type FingerIndex;
		FVector Location;
		bool bMoved;
	};
	void BeginTouch(const ETouchIndex::Type FingerIndex, const FVector Location);
	void EndTouch(const ETouchIndex::Type FingerIndex, const FVector Location);
	void TouchUpdate(const ETouchIndex::Type FingerIndex, const FVector Location);
	TouchData	TouchItem;
	
protected:
	// APawn interface
	virtual void SetupPlayerInputComponent(UInputComponent* InputComponent) override;
	// End of APawn interface

	/* 
	 * Configures input for touchscreen devices if there is a valid touch interface for doing so 
	 *
	 * @param	InputComponent	The input component pointer to bind controls to
	 * @returns true if touch controls were enabled.
	 */
	bool EnableTouchscreenMovement(UInputComponent* InputComponent);

public:
	/** Returns Mesh1P subobject **/
	FORCEINLINE class USkeletalMeshComponent* GetMesh1P() const { return Mesh1P; }
	/** Returns FirstPersonCameraComponent subobject **/
	FORCEINLINE class UCameraComponent* GetFirstPersonCameraComponent() const { return FirstPersonCameraComponent; }

	/** Returns Player Resource Points. */
	FORCEINLINE int GetResourcePts() const { return ResourcePoints; }

	/** Increments Resource Points by the given amount. */
	FORCEINLINE void IncrementResourcePts(int GivenPts) { ResourcePoints += GivenPts; }

	/** Return Warning Event status */
	FORCEINLINE bool IsWarningEventOn() { return WarningEventOn;  }

	/** Return the player's current state */
	FORCEINLINE EPlayerMode GetPlayerState() { return CurrentMode; }

	/** Return the player's current tower choice */
	FORCEINLINE ESelectedTower GetSelectedTower() { return SelectedTower; }
};

