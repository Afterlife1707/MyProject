// Fill out your copyright notice in the Description page of Project Settings.


#include "MyBTTask_FindPathPoint.h"

#include "NPC.h"
#include "NPC_AIController.h"
#include "BehaviorTree/BlackboardComponent.h"

UMyBTTask_FindPathPoint::UMyBTTask_FindPathPoint(FObjectInitializer const& ObjectInitializer) :UBTTask_BlackboardBase{ ObjectInitializer }
{
    NodeName = TEXT("Find Path Point");
}

EBTNodeResult::Type UMyBTTask_FindPathPoint::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    if (auto* const controller = Cast<ANPC_AIController>(OwnerComp.GetAIOwner()))
    {
        //attempt to get the blackboard component from the behaviour tree
        if(auto* const bc = OwnerComp.GetBlackboardComponent())
        {
            //get the current patrol path index from the blackboard
            auto const index = bc->GetValueAsInt(GetSelectedBlackboardKey());

            //get the npc
            if(auto* npc = Cast<ANPC>(controller->GetPawn()))
            {
                if(!npc->GetPatrolPath())
                    return EBTNodeResult::Failed;
                //get the current patrol path vector from the npc - this is the local patrol path actor
                auto const Point = npc->GetPatrolPath()->GetPatrolPoint(index);

                //convert the local vector to global point
                auto const GlobalPoint = npc->GetPatrolPath()->GetActorTransform().TransformPosition(Point);
                bc->SetValueAsVector(PatrolPathVectorKey.SelectedKeyName, GlobalPoint);

                //finish with success
                FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
                return EBTNodeResult::Succeeded;
            }
        }
    }
    return EBTNodeResult::Failed;
}
