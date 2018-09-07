// Fill out your copyright notice in the Description page of Project Settings.

#include "BTService_FindGoal.h"
#include "EnemyAIController.h"
#include "EnemyGoal.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/Blackboard/BlackboardKeyAllTypes.h"

#include "Runtime/Engine/Classes/Engine/Engine.h"

UBTService_FindGoal::UBTService_FindGoal() 
{
	bCreateNodeInstance = true;
}

void UBTService_FindGoal::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) 
{
	// Get AI controller class
	AEnemyAIController *EnemyAI = Cast<AEnemyAIController>(OwnerComp.GetAIOwner());
	
	if (EnemyAI) 
	{
		// Check if there is an Enemy Goal in the world
		TArray<AActor*> FoundGoals;
		UGameplayStatics::GetAllActorsOfClass(GetWorld(), AEnemyGoal::StaticClass(), FoundGoals);

		if (FoundGoals.Num() > 0)
		{
			// Assume there is a single goal for now
			AEnemyGoal *Goal = Cast<AEnemyGoal>(FoundGoals[0]); 

			if (Goal)
			{
				// Attach blackboard key value to the found goal
				OwnerComp.GetBlackboardComponent()->SetValue<UBlackboardKeyType_Object>(EnemyAI->GoalKeyID, Goal);
			}
		}
		else
		{
			GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Blue, "No goal is present");
		}
	}

}

