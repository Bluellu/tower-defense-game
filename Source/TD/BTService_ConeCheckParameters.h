// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Services/BTService_BlackboardBase.h"
#include "BTService_ConeCheckParameters.generated.h"

/**
 * 
 */
UCLASS()
class TD_API UBTService_ConeCheckParameters : public UBTService_BlackboardBase
{
	GENERATED_BODY()

public:
	UBTService_ConeCheckParameters();
	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
	
};
