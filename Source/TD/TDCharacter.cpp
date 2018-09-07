// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#include "TDCharacter.h"
#include "TDProjectile.h"
#include "Animation/AnimInstance.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/InputSettings.h"
#include "HeadMountedDisplayFunctionLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "MotionControllerComponent.h"
#include "XRMotionControllerBase.h" // for FXRMotionControllerBase::RightHandSourceId

#include "Runtime/Engine/Classes/Engine/Engine.h"
#include "DrawDebugHelpers.h"
#include "Runtime/Core/Public/Misc/App.h"
#include "Enemy.h"
#include "DefenseTowerBase.h"
#include "DefenseTower.h"
#include "StickyTower.h"
#include "Runtime/CoreUObject/Public/UObject/ConstructorHelpers.h"
#include "Runtime/Engine/Public/TimerManager.h"


DEFINE_LOG_CATEGORY_STATIC(LogFPChar, Warning, All);

//////////////////////////////////////////////////////////////////////////
// ATDCharacter: The Player class

ATDCharacter::ATDCharacter()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(55.f, 96.0f);

	// set our turn rates for input
	BaseTurnRate = 45.f;
	BaseLookUpRate = 45.f;

	// Create a CameraComponent	
	FirstPersonCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
	FirstPersonCameraComponent->SetupAttachment(GetCapsuleComponent());
	FirstPersonCameraComponent->RelativeLocation = FVector(-39.56f, 1.75f, 64.f); // Position the camera
	FirstPersonCameraComponent->bUsePawnControlRotation = true;

	// Create a mesh component that will be used when being viewed from a '1st person' view (when controlling this pawn)
	Mesh1P = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("CharacterMesh1P"));
	Mesh1P->SetOnlyOwnerSee(true);
	Mesh1P->SetupAttachment(FirstPersonCameraComponent);
	Mesh1P->bCastDynamicShadow = false;
	Mesh1P->CastShadow = false;
	Mesh1P->RelativeRotation = FRotator(1.9f, -19.19f, 5.2f);
	Mesh1P->RelativeLocation = FVector(-0.5f, -4.4f, -155.7f);

	// Create a gun mesh component
	FP_Gun = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("FP_Gun"));
	FP_Gun->SetOnlyOwnerSee(true);			// only the owning player will see this mesh
	FP_Gun->bCastDynamicShadow = false;
	FP_Gun->CastShadow = false;
	// FP_Gun->SetupAttachment(Mesh1P, TEXT("GripPoint"));
	FP_Gun->SetupAttachment(RootComponent);

	FP_MuzzleLocation = CreateDefaultSubobject<USceneComponent>(TEXT("MuzzleLocation"));
	FP_MuzzleLocation->SetupAttachment(FP_Gun);
	FP_MuzzleLocation->SetRelativeLocation(FVector(0.2f, 48.4f, -10.6f));

	// Default offset from the character location for projectiles to spawn
	GunOffset = FVector(100.0f, 0.0f, 10.0f);

	// Create VR Controllers.
	R_MotionController = CreateDefaultSubobject<UMotionControllerComponent>(TEXT("R_MotionController"));
	R_MotionController->MotionSource = FXRMotionControllerBase::RightHandSourceId;
	R_MotionController->SetupAttachment(RootComponent);
	L_MotionController = CreateDefaultSubobject<UMotionControllerComponent>(TEXT("L_MotionController"));
	L_MotionController->SetupAttachment(RootComponent);

	// Create a gun and attach it to the right-hand VR controller.
	// Create a gun mesh component
	VR_Gun = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("VR_Gun"));
	VR_Gun->SetOnlyOwnerSee(true);			// only the owning player will see this mesh
	VR_Gun->bCastDynamicShadow = false;
	VR_Gun->CastShadow = false;
	VR_Gun->SetupAttachment(R_MotionController);
	VR_Gun->SetRelativeRotation(FRotator(0.0f, -90.0f, 0.0f));

	VR_MuzzleLocation = CreateDefaultSubobject<USceneComponent>(TEXT("VR_MuzzleLocation"));
	VR_MuzzleLocation->SetupAttachment(VR_Gun);
	VR_MuzzleLocation->SetRelativeLocation(FVector(0.000004, 53.999992, 10.000000));
	VR_MuzzleLocation->SetRelativeRotation(FRotator(0.0f, 90.0f, 0.0f));		// Counteract the rotation of the VR gun model.

	// Uncomment the following line to turn motion controllers on by default:
	//bUsingMotionControllers = true;

	GunStrength = 10; // Default HP-based gun damage
	GunRangeMultiplier = 1000.f; // Defines how far the player can shoot
	ResourcePoints = 50; // Initialize resource storage

	// Player state
	CurrentMode = EPlayerMode::AttackMode; // Default player Mode for shooting
	AttachedTower = 0; // Initialize defense tower reference to be empty 

	// Import Defense Tower Blueprints
	// Turret class (DefenseTower_BP)
	static ConstructorHelpers::FObjectFinder<UClass> TowerBPFind(TEXT("Class'/Game/FirstPersonCPP/Blueprints/DefenseTower_BP.DefenseTower_BP_C'"));
	TurretBPClass = TowerBPFind.Object;

	//Sticky Tower class (StickyTower_BP)
	static ConstructorHelpers::FObjectFinder<UClass> StickyBPFind(TEXT("Class'/Game/FirstPersonCPP/Blueprints/StickyTower_BP.StickyTower_BP_C'"));
	StickyBPClass = StickyBPFind.Object;

	// Set default selected tower
	SelectedTower = ESelectedTower::Turret;

	WarningEventOn = false;
}

void ATDCharacter::BeginPlay()
{
	// Call the base class  
	Super::BeginPlay();

	//Attach gun mesh component to Skeleton, doing it here because the skeleton is not yet created in the constructor
	FP_Gun->AttachToComponent(Mesh1P, FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true), TEXT("GripPoint"));

	// Show or hide the two versions of the gun based on whether or not we're using motion controllers.
	if (bUsingMotionControllers)
	{
		VR_Gun->SetHiddenInGame(false, true);
		Mesh1P->SetHiddenInGame(true, true);
	}
	else
	{
		VR_Gun->SetHiddenInGame(true, true);
		Mesh1P->SetHiddenInGame(false, true);
	}
}

//////////////////////////////////////////////////////////////////////////
// Input

void ATDCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// set up gameplay key bindings
	check(PlayerInputComponent);

	// Bind jump events
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);

	// Bind fire event
	PlayerInputComponent->BindAction("Fire", IE_Pressed, this, &ATDCharacter::OnFire);

	// Bind Mode change events
	PlayerInputComponent->BindAction("BuildMode", IE_Pressed, this, &ATDCharacter::EnterBuildMode);
	PlayerInputComponent->BindAction("BuildMode", IE_Released, this, &ATDCharacter::LeaveBuildMode);

	// Bind tower changing event
	PlayerInputComponent->BindAction("SwitchTower", IE_Pressed, this, &ATDCharacter::SwitchTower);

	// Enable touchscreen input
	EnableTouchscreenMovement(PlayerInputComponent);

	PlayerInputComponent->BindAction("ResetVR", IE_Pressed, this, &ATDCharacter::OnResetVR);

	// Bind movement events
	PlayerInputComponent->BindAxis("MoveForward", this, &ATDCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &ATDCharacter::MoveRight);

	// We have 2 versions of the rotation bindings to handle different kinds of devices differently
	// "turn" handles devices that provide an absolute delta, such as a mouse.
	// "turnrate" is for devices that we choose to treat as a rate of change, such as an analog joystick
	PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("TurnRate", this, &ATDCharacter::TurnAtRate);
	PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("LookUpRate", this, &ATDCharacter::LookUpAtRate);

}

/* Spawn DefenseTower and attach it to the player. */
void ATDCharacter::SpawnDefenseTower()
{
	if (!AttachedTower)
	{
		FActorSpawnParameters SpawnParams;
		SpawnParams.Owner = this;
		FVector PlayerLocation = this->GetActorLocation();
		FVector Offset = (this->GetActorForwardVector() * 100.f) + FVector(0.f, 0.f, -50.f);
		FVector SpawnLocation = this->GetActorLocation() + Offset;
		FRotator Rotator = FRotator(0);

		// Spawn Tower and attach to the player
		switch (SelectedTower)
		{
		case (ESelectedTower::Turret):
			AttachedTower = GetWorld()->SpawnActor<ADefenseTower>(TurretBPClass, SpawnLocation, Rotator, SpawnParams);
			break;
		case (ESelectedTower::Sticky):
			AttachedTower = GetWorld()->SpawnActor<AStickyTower>(StickyBPClass, SpawnLocation, Rotator, SpawnParams);
		}

		if (AttachedTower) // Success
		{
			ADefenseTowerBase *Tower = (Cast<ADefenseTowerBase>(AttachedTower));
			Tower->SetCreator(this); // Give tower a reference to this player
		}
	}
}

void ATDCharacter::EnterBuildMode()
{
	CurrentMode = EPlayerMode::BuildMode;

	// Spawn the default DefenseTower attached to the player
	SpawnDefenseTower();
}

/* Return to Attack Mode and remove attached Tower */
void ATDCharacter::LeaveBuildMode()
{
	CurrentMode = EPlayerMode::AttackMode;

	if (AttachedTower)
	{
		AttachedTower->Destroy();
		AttachedTower = 0;
	}
}

/* Handles trigger function depending on current Mode. */
void ATDCharacter::OnFire()
{
	if (CurrentMode == EPlayerMode::AttackMode)
		AttackModeFire();
	else if (CurrentMode == EPlayerMode::BuildMode)
		BuildModeFire();
}

/* Fires a projectile and handles damage. */
void ATDCharacter::AttackModeFire()
{
	// Line Tracing
	FHitResult Hit;
	FVector Start = FP_Gun->GetComponentLocation();
	FVector ForwardVector = FirstPersonCameraComponent->GetForwardVector();
	ForwardVector.Normalize();
	FVector End = ((GunRangeMultiplier * ForwardVector) + Start);
	FCollisionQueryParams CollisionParams;

	// Draw debug line to visualize shooting range (placeholder for a particle effect)
	DrawDebugLine(GetWorld(), Start, End, FColor::Blue, false, 0.1f, (uint8)('\000'), 0.3f);

	// If line trace detects a hit, check actor type
	if (GetWorld()->LineTraceSingleByChannel(Hit, Start, End, ECC_Visibility, CollisionParams))
	{
		if (Hit.bBlockingHit)
		{
			AActor* CollidedWith = Hit.GetActor();

			// If Enemy, give out damage
			if (CollidedWith->IsA(AEnemy::StaticClass()))
			{
				AEnemy *HitEnemy = Cast<AEnemy>(CollidedWith);

				// Inflict damage and obtain resource points (0 if enemy still alive).
				int ResourceReceived = HitEnemy->ReceiveDamage(GunStrength);
				ResourcePoints += ResourceReceived;
			}
		}
	}

	// try and play the sound if specified
	if (FireSound != NULL)
	{
		UGameplayStatics::PlaySoundAtLocation(this, FireSound, GetActorLocation());
	}

	// try and play a firing animation if specified
	if (FireAnimation != NULL)
	{
		// Get the animation object for the arms mesh
		UAnimInstance* AnimInstance = Mesh1P->GetAnimInstance();
		if (AnimInstance != NULL)
		{
			AnimInstance->Montage_Play(FireAnimation, 1.f);
		}
	}
}

/* Sets down a Defense Tower */
void ATDCharacter::BuildModeFire()
{
	if (AttachedTower)
	{
		ADefenseTowerBase *DefenseTower = (Cast<ADefenseTowerBase>(AttachedTower));

		if (ResourcePoints >= DefenseTower->GetCost())
		{
			// Player can buy and place this tower
			if (DefenseTower->IsUnobstructed())
			{
				DefenseTower->Deploy();
				AttachedTower = 0; // Detach tower from player.

				//Set deployed tower slightly higher to avoid clipping
				DefenseTower->SetActorLocation(DefenseTower->GetActorLocation() + FVector(0.0f, 0.0f, 0.1f));
				DefenseTower->SetActorScale3D(DefenseTower->GetActorScale() + FVector(0.01f, 0.01f, 0.0f));

				ResourcePoints -= DefenseTower->GetCost();

				//Spawn new attached tower
				SpawnDefenseTower();
			}
		}

		// Player cannot buy this tower
		else
		{
			// Start warning event (for HUD to detect)
			OnWarningEvent();
		}
	}
}

/* Change active defense tower (Build Mode only) */
void ATDCharacter::SwitchTower()
{
	if (CurrentMode == EPlayerMode::BuildMode)
	{
		// Clear current tower
		AttachedTower->Destroy();
		AttachedTower = 0;

		// Switch to next tower, and spawn a new one
		switch (SelectedTower)
		{
		case ESelectedTower::Turret:
		{
			SelectedTower = ESelectedTower::Sticky;
			SpawnDefenseTower();
			break;
		}
		case ESelectedTower::Sticky:
		{
			SelectedTower = ESelectedTower::Turret;
			SpawnDefenseTower();
		}
		}
	}
}

/* Starts timer for warning display */
void ATDCharacter::OnWarningEvent()
{
	// Activate Warning Event
	WarningEventOn = true;

	FTimerHandle TimerHandle;
	FTimerDelegate TimerDelegate;
	TimerDelegate.BindUFunction(this, FName("EndWarningEvent"));

	// Spawn new Enemy after a small delay
	GetWorldTimerManager().SetTimer(TimerHandle, TimerDelegate, 2.0f, false);
}

/** To make warning on screen disappear. */
void ATDCharacter::EndWarningEvent()
{
	WarningEventOn = false;
}


void ATDCharacter::OnResetVR()
{
	UHeadMountedDisplayFunctionLibrary::ResetOrientationAndPosition();
}


void ATDCharacter::BeginTouch(const ETouchIndex::Type FingerIndex, const FVector Location)
{
	if (TouchItem.bIsPressed == true)
	{
		return;
	}
	if ((FingerIndex == TouchItem.FingerIndex) && (TouchItem.bMoved == false))
	{
		OnFire();
	}
	TouchItem.bIsPressed = true;
	TouchItem.FingerIndex = FingerIndex;
	TouchItem.Location = Location;
	TouchItem.bMoved = false;
}


void ATDCharacter::EndTouch(const ETouchIndex::Type FingerIndex, const FVector Location)
{
	if (TouchItem.bIsPressed == false)
	{
		return;
	}
	TouchItem.bIsPressed = false;
}


void ATDCharacter::MoveForward(float Value)
{
	if (Value != 0.0f)
	{
		// add movement in that direction
		AddMovementInput(GetActorForwardVector(), Value);
	}
}


void ATDCharacter::MoveRight(float Value)
{
	if (Value != 0.0f)
	{
		// add movement in that direction
		AddMovementInput(GetActorRightVector(), Value);
	}
}


void ATDCharacter::TurnAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}


void ATDCharacter::LookUpAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
}


bool ATDCharacter::EnableTouchscreenMovement(class UInputComponent* PlayerInputComponent)
{
	if (FPlatformMisc::SupportsTouchInput() || GetDefault<UInputSettings>()->bUseMouseForTouch)
	{
		PlayerInputComponent->BindTouch(EInputEvent::IE_Pressed, this, &ATDCharacter::BeginTouch);
		PlayerInputComponent->BindTouch(EInputEvent::IE_Released, this, &ATDCharacter::EndTouch);

		return true;
	}
	
	return false;
}

