// Fill out your copyright notice in the Description page of Project Settings.

#include "BTTask_MoveToWaypoint.h"
#include "EnemyAIController.h"
#include "Waypoint.h"
#include "Engine/World.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/Blackboard/BlackboardKeyAllTypes.h"
#include "Enemy.h"

#include "Runtime/Engine/Classes/Engine/Engine.h"

EBTNodeResult::Type UBTTask_MoveToWaypoint::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AEnemyAIController *EnemyAI = Cast<AEnemyAIController>(OwnerComp.GetAIOwner());
	AActor *Waypoint = Cast<AWaypoint>(OwnerComp.GetBlackboardComponent()->GetValue<UBlackboardKeyType_Object>(EnemyAI->WaypointKeyID));

	if (Waypoint )
	{
		EnemyAI->MoveToActor(Waypoint, -1.0f, true, true, true, 0, true);
		EnemyAI->MoveToLocation(Waypoint->GetActorLocation(), -1.0f, true, true, false, true, 0);

		return EBTNodeResult::Succeeded;
	} 
	
	return EBTNodeResult::Failed;
}
