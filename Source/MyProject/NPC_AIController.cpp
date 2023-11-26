// Fill out your copyright notice in the Description page of Project Settings.


#include "NPC_AIController.h"

#include "MyProjectCharacter.h"
#include "NPC.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Hearing.h"
#include "Perception/AISenseConfig_Sight.h"

ANPC_AIController::ANPC_AIController(FObjectInitializer const& ObjectInitializer)
{
    SetupPerceptionSystem();
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

void ANPC_AIController::SetupPerceptionSystem()
{
    SightConfig = CreateDefaultSubobject<UAISenseConfig_Sight>(TEXT("Sight Config"));
    if(SightConfig)
    {
        SetPerceptionComponent(*CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("Perception Component")));
        SightConfig->SightRadius = 1000.f;
        SightConfig->LoseSightRadius = SightConfig->SightRadius + 25.f;
        SightConfig->PeripheralVisionAngleDegrees = 90.f;
        SightConfig->SetMaxAge(5.f);
        SightConfig->AutoSuccessRangeFromLastSeenLocation = 1020.f;
        SightConfig->DetectionByAffiliation.bDetectEnemies = true;
        SightConfig->DetectionByAffiliation.bDetectFriendlies = true;
        SightConfig->DetectionByAffiliation.bDetectNeutrals = true;

        GetPerceptionComponent()->SetDominantSense(*SightConfig->GetSenseImplementation());
        GetPerceptionComponent()->OnTargetPerceptionUpdated.AddDynamic(this, &ANPC_AIController::OnTargetDetected);
        GetPerceptionComponent()->ConfigureSense(*SightConfig);
        SightID = SightConfig->GetSenseID();
    }

    HearingConfig = CreateDefaultSubobject<UAISenseConfig_Hearing>(TEXT("Hearing Config"));
    if(HearingConfig)
    {
        HearingConfig->DetectionByAffiliation.bDetectFriendlies = true;
        HearingConfig->DetectionByAffiliation.bDetectNeutrals = true;
        HearingConfig->DetectionByAffiliation.bDetectEnemies = true;
        HearingConfig->HearingRange = 2500.f;

        GetPerceptionComponent()->OnTargetPerceptionUpdated.AddDynamic(this, &ANPC_AIController::OnTargetDetected);
        GetPerceptionComponent()->ConfigureSense(*HearingConfig);
        HearID = HearingConfig->GetSenseID();
    }
}

void ANPC_AIController::OnTargetDetected(AActor* Actor, FAIStimulus const Stimulus)
{
    if(auto* const ch = Cast<AMyProjectCharacter>(Actor))
    {
        if (Stimulus.Type== SightID && ch->IsPlayerInLight())
        {
            UE_LOG(LogTemp, Warning, TEXT("Seen by NPC"));
            GetBlackboardComponent()->SetValueAsBool("CanSeePlayer", Stimulus.WasSuccessfullySensed());
            ch->SpottedByNPC();
        }
        else if (Stimulus.Type == HearID)
        {
            UE_LOG(LogTemp, Warning, TEXT("Heard by NPC"));
            GetBlackboardComponent()->SetValueAsBool("CanHearPlayer", Stimulus.WasSuccessfullySensed());
            GetBlackboardComponent()->SetValueAsVector("LastHeardLocation", Stimulus.StimulusLocation);
            ch->HeardByNPC();
        }
    }
}
