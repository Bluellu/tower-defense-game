// Fill out your copyright notice in the Description page of Project Settings.

#include "BTTask_CheckMovementFinished.h"
#include "EnemyAIController.h"
#include "Engine/World.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/Blackboard/BlackboardKeyAllTypes.h"


EBTNodeResult::Type UBTTask_CheckMovementFinished::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AEnemyAIController *EnemyAI = Cast<AEnemyAIController>(OwnerComp.GetAIOwner());

	if (EnemyAI->GetMoveStatus() == EPathFollowingStatus::Idle)
	{
		// Enemy is not moving
		OwnerComp.GetBlackboardComponent()->ClearValue(EnemyAI->WaypointKeyID);
		return EBTNodeResult::Succeeded;
	}
	return EBTNodeResult::Failed;
}

