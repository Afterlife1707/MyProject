// Fill out your copyright notice in the Description page of Project Settings.


#include "MyBTTask_ChasePlayer.h"

#include "NPC_AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"

UMyBTTask_ChasePlayer::UMyBTTask_ChasePlayer(FObjectInitializer const& ObjectInitializer) :UBTTask_BlackboardBase{ ObjectInitializer }
{
    NodeName = TEXT("Chase Player");
}

EBTNodeResult::Type UMyBTTask_ChasePlayer::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    //get target location from blackboard via the NPC's controller
    if (auto* const controller = Cast<ANPC_AIController>(OwnerComp.GetAIOwner()))
    {
        auto const PlayerLocation = OwnerComp.GetBlackboardComponent()->GetValueAsVector(GetSelectedBlackboardKey());

        //move to the player's location
        UAIBlueprintHelperLibrary::SimpleMoveToLocation(controller, PlayerLocation);

        //finish with success
        FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
        return EBTNodeResult::Succeeded;
    }
    return EBTNodeResult::Failed;
}
