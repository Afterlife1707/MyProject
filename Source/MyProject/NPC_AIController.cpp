// Fill out your copyright notice in the Description page of Project Settings.


#include "NPC_AIController.h"

#include "NPC.h"

ANPC_AIController::ANPC_AIController(FObjectInitializer const& ObjectInitializer)
{

}

void ANPC_AIController::OnPossess(APawn* InPawn)
{
    Super::OnPossess(InPawn);
    if(ANPC* const NPC = Cast<ANPC>(InPawn))
    {
        if(UBehaviorTree* const tree = NPC->GetBehaviourTree())
        {
            UBlackboardComponent* b;
            UseBlackboard(tree->BlackboardAsset, b);
            Blackboard = b;
            RunBehaviorTree(tree);
        }
    }
}
