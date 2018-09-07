// Fill out your copyright notice in the Description page of Project Settings.

#include "BTService_ConeCheckParameters.h"
#include "EnemyAIController.h"
#include "EnemyGoal.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/Blackboard/BlackboardKeyAllTypes.h"

#include "Runtime/Engine/Classes/Engine/Engine.h"

UBTService_ConeCheckParameters::UBTService_ConeCheckParameters()
{
	bCreateNodeInstance = true;
}

void UBTService_ConeCheckParameters::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	// Get AI controller class
	AEnemyAIController *EnemyAI = Cast<AEnemyAIController>(OwnerComp.GetAIOwner());

	if (EnemyAI)
	{
		FVector EnemyLocation = EnemyAI->GetPawn()->GetActorLocation();

		// Set Enemy AI position to key
		OwnerComp.GetBlackboardComponent()->SetValue<UBlackboardKeyType_Vector>(EnemyAI->AIPositionKeyID, 
																				EnemyLocation);

		// Set Goal Position to key
		TArray<AActor*> FoundGoals;
		UGameplayStatics::GetAllActorsOfClass(GetWorld(), AEnemyGoal::StaticClass(), FoundGoals);

		if (FoundGoals.Num() > 0)
		{
			AEnemyGoal *Goal = Cast<AEnemyGoal>(FoundGoals[0]);
			if (Goal)
			{
				// Set goal direction vector to key
				FVector GoalDirection = Goal->GetActorLocation() - EnemyLocation;
				OwnerComp.GetBlackboardComponent()->SetValue<UBlackboardKeyType_Vector>(EnemyAI->GoalDirectionKeyID, GoalDirection);
			}
		}
	}
}




