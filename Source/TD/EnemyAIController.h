// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "EnemyAIController.generated.h"


/**
 * 
 */
UCLASS()
class TD_API AEnemyAIController : public AAIController
{
	GENERATED_BODY()

	/* Data container for Behavior Tree to use for decision making */
	UPROPERTY(transient)
	class UBlackboardComponent *BlackboardComponent;

	/* The Behavior Tree Component */
	UPROPERTY(transient)
	class UBehaviorTreeComponent *BTComponent;

	public:
		AEnemyAIController();

		virtual void Possess(APawn *InPawn) override;

		/* A Key ID to identify this Enemy's goal within the Behavior Tree */
		uint8 GoalKeyID;

		/* A Key ID to identify a waypoint. */
		uint8 WaypointKeyID;

		/* Identify this controller's position. */
		uint8 AIPositionKeyID;

		/* Identify  the direction vector to a Goal */
		uint8 GoalDirectionKeyID;
};
