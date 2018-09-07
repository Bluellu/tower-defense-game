// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Decorators/BTDecorator_ConditionalLoop.h"
#include "BTDecorator_CLoopUntilSuccess.generated.h"

/**
 * 
 */
UCLASS()
class TD_API UBTDecorator_CLoopUntilSuccess : public UBTDecorator_ConditionalLoop
{
	GENERATED_BODY()

		void OnNodeDeactivation(FBehaviorTreeSearchData& SearchData, EBTNodeResult::Type NodeResult) override;
	
};
