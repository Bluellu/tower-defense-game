// Fill out your copyright notice in the Description page of Project Settings.

#include "BTService_FindWaypoint.h"

#include "EnemyAIController.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/Blackboard/BlackboardKeyAllTypes.h"
#include "Enemy.h"
#include "Runtime/Engine/Classes/Kismet/KismetSystemLibrary.h"
#include "Components/SphereComponent.h"
#include "Waypoint.h"
#include "EnemyGoal.h"

#include "Runtime/Engine/Classes/Engine/Engine.h"


UBTService_FindWaypoint::UBTService_FindWaypoint()
{
	bCreateNodeInstance = true;
}

void UBTService_FindWaypoint::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	// Get AI controller class
	AEnemyAIController *EnemyAI = Cast<AEnemyAIController>(OwnerComp.GetAIOwner());

	if (EnemyAI)
	{
		// Get Pawn controlled by the Enemy AI Controller
		AEnemy *EnemyControlled = Cast<AEnemy>(EnemyAI->GetPawn());

		float HalfAngle = EnemyControlled->GetConeHalfAngle(); // The field of movement for this enemy

		// Get all waypoints within this AI's sensing sphere
		TArray<AActor*> FoundWaypoints;
		EnemyControlled->GetSensingSphere()->GetOverlappingActors(FoundWaypoints);

		// Find Goal
		TArray<AActor*> FoundGoals;
		UGameplayStatics::GetAllActorsOfClass(GetWorld(), AEnemyGoal::StaticClass(), FoundGoals);
		AEnemyGoal *Goal;
		if (FoundGoals.Num() > 0)
			Goal = Cast<AEnemyGoal>(FoundGoals[0]);
		else
			Goal = 0;

		// There are Waypoints available and the Goal exists
		if (FoundWaypoints.Num() > 0 && Goal)
		{
			// Find all Waypoints within this enemy's view cone pointing towards the goal

			// Get enemy location
			FVector EnemyLocation = EnemyAI->GetPawn()->GetActorLocation();

			// Get Goal direction
			FVector GoalDir = Goal->GetActorLocation() - EnemyLocation;
			GoalDir.Normalize();

			// Get all valid Waypoints (must be within 90 degrees of the GoalDir vector)
			auto WaypointsInRange = FoundWaypoints.FilterByPredicate([&](const AActor* Waypt)
			{
				FVector WayptDir = Waypt->GetActorLocation() - EnemyLocation;
				WayptDir.Normalize();
				float Angle = FMath::RadiansToDegrees(FMath::Acos(FVector::DotProduct(GoalDir, WayptDir)));
				//GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Blue, FString::SanitizeFloat(Angle));
				return Angle < HalfAngle;
			});

			if (WaypointsInRange.Num() > 0)
			{
				int RandomIndex = FMath::RandRange(0, WaypointsInRange.Num() - 1);
				AWaypoint *ValidWaypt = Cast<AWaypoint>(WaypointsInRange[RandomIndex]);

				AWaypoint *PreviousWaypt = Cast<AWaypoint>(
					OwnerComp.GetBlackboardComponent()->GetValue<UBlackboardKeyType_Object>(EnemyAI->WaypointKeyID));

				// We have found a waypoint and it is not the one the enemy is at
				if (ValidWaypt && (ValidWaypt != PreviousWaypt))
				{
					// Attach blackboard key value to the found Waypoint
					OwnerComp.GetBlackboardComponent()->SetValue<UBlackboardKeyType_Object>(EnemyAI->WaypointKeyID, 
																							ValidWaypt);
				}
			}
			else
			{
				// No VALID waypoint present
				OwnerComp.GetBlackboardComponent()->ClearValue(EnemyAI->WaypointKeyID);
			}
		}
		else
		{
			// No waypoint present at all
			OwnerComp.GetBlackboardComponent()->ClearValue(EnemyAI->WaypointKeyID);
		} 
	}

}