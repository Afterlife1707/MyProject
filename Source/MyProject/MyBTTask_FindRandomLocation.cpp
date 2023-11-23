// Fill out your copyright notice in the Description page of Project Settings.


#include "MyBTTask_FindRandomLocation.h"

#include "NavigationSystem.h"
#include "NPC_AIController.h"
#include "BehaviorTree/BlackboardComponent.h"

UMyBTTask_FindRandomLocation::UMyBTTask_FindRandomLocation(FObjectInitializer const& ObjectInitializer) :UBTTask_BlackboardBase{ ObjectInitializer }
{
    NodeName = TEXT("Find Random Location in NavMesh");
}

EBTNodeResult::Type UMyBTTask_FindRandomLocation::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    //get AI controller and its npc
    if(ANPC_AIController* const controller=Cast<ANPC_AIController>(OwnerComp.GetAIOwner()))
    {
        if(auto* const NPC = controller->GetPawn())
        {
            //obtain npc location to use as an origin
            auto const Origin = NPC->GetActorLocation();

            //get the navigation system and generate a random location
            if (auto* const NavSys = UNavigationSystemV1::GetCurrent(GetWorld()))
            {
                FNavLocation Loc;
                if(NavSys->GetRandomPointInNavigableRadius(Origin, SearchRadius, Loc))
                {
                    OwnerComp.GetBlackboardComponent()->SetValueAsVector(GetSelectedBlackboardKey(), Loc.Location);
                }

                //finish with success
                FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
                return EBTNodeResult::Succeeded;
            }
        }
        return EBTNodeResult::Failed;
    }


    return Super::ExecuteTask(OwnerComp, NodeMemory);
}
