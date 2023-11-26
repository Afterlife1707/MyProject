// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "Perception/AIPerceptionTypes.h"
#include "NPC_AIController.generated.h"

/**
 * 
 */
UCLASS()
class MYPROJECT_API ANPC_AIController : public AAIController
{
	GENERATED_BODY()

public:
	explicit ANPC_AIController(FObjectInitializer const& ObjectInitializer);

protected:
    virtual void OnPossess(APawn* InPawn) override;

private:
	class UAISenseConfig_Sight* SightConfig;
	class UAISenseConfig_Hearing* HearingConfig;

	void SetupPerceptionSystem();
	FAISenseID SightID; 
	FAISenseID HearID;
	UFUNCTION()
	void OnTargetDetected(AActor* Actor, FAIStimulus const Stimulus);
};
