// Fill out your copyright notice in the Description page of Project Settings.

#include "EnemyAIController.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/Blackboard/BlackboardKeyAllTypes.h"
#include "Enemy.h"
#include "EnemyGoal.h"
#include "Kismet/GameplayStatics.h"

AEnemyAIController::AEnemyAIController()
{
	BlackboardComponent = CreateDefaultSubobject<UBlackboardComponent>(TEXT("Blackboard Component"));
	BTComponent = CreateDefaultSubobject<UBehaviorTreeComponent>(TEXT("Behavior Tree Component"));
}

/* Overriden Possess function. */
void AEnemyAIController::Possess(APawn *InPawn)
{
	Super::Possess(InPawn);
	AEnemy *EnemyPawn = Cast<AEnemy>(InPawn);

	/* Check that Enemy exists and possesses a Behavior Tree object before possession */
	if (EnemyPawn && EnemyPawn->EnemyBehavior)
	{
		BlackboardComponent->InitializeBlackboard(*EnemyPawn->EnemyBehavior->BlackboardAsset);
		GoalKeyID = BlackboardComponent->GetKeyID("Goal");
		WaypointKeyID = BlackboardComponent->GetKeyID("Waypoint");
		AIPositionKeyID = BlackboardComponent->GetKeyID("AIPosition");
		GoalDirectionKeyID = BlackboardComponent->GetKeyID("GoalDirection");

		// Set this Enemy's goal 
		TArray<AActor*> FoundGoals;
		UGameplayStatics::GetAllActorsOfClass(GetWorld(), AEnemyGoal::StaticClass(), FoundGoals);

		if (FoundGoals.Num() > 0)
		{
			// Assume there is a single goal for now
			AActor *Goal = FoundGoals[0];
			this->SetFocus(Goal, EAIFocusPriority::Gameplay);
		}

		BTComponent->StartTree(*EnemyPawn->EnemyBehavior);
	}
}

