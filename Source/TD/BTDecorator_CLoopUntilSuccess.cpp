// Fill out your copyright notice in the Description page of Project Settings.

#include "BTDecorator_CLoopUntilSuccess.h"
#include "GameFramework/Actor.h"
#include "VisualLogger/VisualLogger.h"
#include "BehaviorTree/BTCompositeNode.h"

// A Version of The conditional loop Decorator that breaks upon success
void UBTDecorator_CLoopUntilSuccess::OnNodeDeactivation(FBehaviorTreeSearchData& SearchData, EBTNodeResult::Type NodeResult)
{
	if ((NodeResult != EBTNodeResult::Aborted) && (NodeResult != EBTNodeResult::Succeeded)) // Break loop on Success
	{
		const UBlackboardComponent* BlackboardComp = SearchData.OwnerComp.GetBlackboardComponent();
		const bool bEvalResult = BlackboardComp && EvaluateOnBlackboard(*BlackboardComp);
		UE_VLOG(SearchData.OwnerComp.GetOwner(), LogBehaviorTree, Verbose, TEXT("Loop condition: %s -> %s"),
				bEvalResult ? TEXT("true") : TEXT("false"), (bEvalResult != IsInversed()) ? TEXT("run again!") : TEXT("break"));

		if (bEvalResult != IsInversed())
		{
			GetParentNode()->SetChildOverride(SearchData, GetChildIndex());
		}
	}
}


