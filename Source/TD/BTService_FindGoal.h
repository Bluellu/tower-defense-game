// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Services/BTService_BlackboardBase.h"
#include "BTService_FindGoal.generated.h"

/**
 * 
 */
UCLASS()
class TD_API UBTService_FindGoal : public UBTService_BlackboardBase
{
	GENERATED_BODY()

public:
	UBTService_FindGoal();
	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

};
