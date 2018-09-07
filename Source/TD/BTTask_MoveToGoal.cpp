// Fill out your copyright notice in the Description page of Project Settings.

#include "BTTask_MoveToGoal.h"

#include "EnemyAIController.h"
#include "EnemyGoal.h"
#include "Engine/World.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/Blackboard/BlackboardKeyAllTypes.h"

#include "Runtime/Engine/Classes/Engine/Engine.h"

EBTNodeResult::Type UBTTask_MoveToGoal::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) 
{
	AEnemyAIController *EnemyAI = Cast<AEnemyAIController>(OwnerComp.GetAIOwner());

	AActor *Goal = Cast<AEnemyGoal>(OwnerComp.GetBlackboardComponent()->GetValue<UBlackboardKeyType_Object>(EnemyAI->GoalKeyID));

	if (Goal)
	{
		EnemyAI->MoveToActor(Goal, -1.0f, true, true, true, 0, true);
		return EBTNodeResult::Succeeded;
	}
	else
	{
		return EBTNodeResult::Failed;
	}
	return EBTNodeResult::Failed;
}

