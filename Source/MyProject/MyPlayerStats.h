// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "MyPlayerStats.generated.h"

class UMyUserWidget;
UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class MYPROJECT_API UMyPlayerStats : public UActorComponent
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Stats, meta = (AllowPrivateAccess = "true"));
	int ShadowCharges = 3;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Stats, meta = (AllowPrivateAccess = "true"));
    int MAX_CHARGES = 3;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Stats, meta = (AllowPrivateAccess = "true"));
	float ChargeRefillTime = 25.f;
	float t;

    UMyUserWidget* MyUserWidget;
public:	
	// Sets default values for this component's properties
	UMyPlayerStats();
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	int GetCharges() const;
	void SetUserWidget(UMyUserWidget* UserWidget);
	void UseCharge();
	void RefillCharge(float deltaTime);
protected:
	// Called when the game starts
	virtual void BeginPlay() override;
};
