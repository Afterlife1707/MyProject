// Fill out your copyright notice in the Description page of Project Settings.


#include "MyBTTask_ClearBlackboardKey.h"

UMyBTTask_ClearBlackboardKey::UMyBTTask_ClearBlackboardKey(FObjectInitializer const& ObjectInitializer)
{
    NodeName = TEXT("Clear Blackboard Key");
}

EBTNodeResult::Type UMyBTTask_ClearBlackboardKey::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
   /* UBlackboardComponent
    BlackboardKey->GetSelectedKeyID()
    if()
    {

        FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
        return EBTNodeResult::Succeeded;
    }*/

    return EBTNodeResult::Failed;
}
