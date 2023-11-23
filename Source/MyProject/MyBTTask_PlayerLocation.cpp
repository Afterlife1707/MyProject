// Fill out your copyright notice in the Description page of Project Settings.


#include "MyBTTask_PlayerLocation.h"

#include "NavigationSystem.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"

UMyBTTask_PlayerLocation::UMyBTTask_PlayerLocation(FObjectInitializer const& ObjectInitializer) :UBTTask_BlackboardBase{ ObjectInitializer }
{
    NodeName = TEXT("Find Player Location");
}

EBTNodeResult::Type UMyBTTask_PlayerLocation::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    //get player character
    if(auto* const Player = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0))
    {
        //get player location to use an origin
        auto const PlayerLocation = Player->GetActorLocation();
        if(SearchRandom)
        {
            FNavLocation Loc;
            //get the navigation system and generate a random location near the player
            if(auto* const NavSys = UNavigationSystemV1::GetCurrent(GetWorld()))
            {
                if(NavSys->GetRandomPointInNavigableRadius(PlayerLocation, SearchRadius, Loc))
                {
                    OwnerComp.GetBlackboardComponent()->SetValueAsVector(GetSelectedBlackboardKey(), Loc.Location);
                    FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
                    return EBTNodeResult::Succeeded;
                }
            }
        }
        else
        {
            OwnerComp.GetBlackboardComponent()->SetValueAsVector(GetSelectedBlackboardKey(), PlayerLocation);
            FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
            return EBTNodeResult::Succeeded;
        }
    }

    return EBTNodeResult::Failed;
}
