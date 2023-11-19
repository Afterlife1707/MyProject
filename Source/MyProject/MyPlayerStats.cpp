// Fill out your copyright notice in the Description page of Project Settings.


#include "MyPlayerStats.h"
#include "MyUserWidget.h"

// Sets default values for this component's properties
UMyPlayerStats::UMyPlayerStats()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}

int UMyPlayerStats::GetTPCharges() const
{
    return ShadowTPCharges;
}

void UMyPlayerStats::SetUserWidget(UMyUserWidget* UserWidget)
{
	MyUserWidget = UserWidget;
}

void UMyPlayerStats::UseTPCharge()
{
	if (MyUserWidget)
	{
		MyUserWidget->UseShadowCharge();
		ShadowTPCharges--;
	}
}


// Called when the game starts
void UMyPlayerStats::BeginPlay()
{
	Super::BeginPlay();

	t = 0;
}


// Called every frame
void UMyPlayerStats::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if(ShadowTPCharges<MAX_TP_CHARGES)
	{
		if(t>=TPRechargeTime)
		{
			ShadowTPCharges++;
			MyUserWidget->ShadowChargedRefilled();
			t = 0;
		}
		else
		{
			t += 1 * DeltaTime;
			RechargeTP(DeltaTime);
		}
	}
	if (ShadowTPCharges > MAX_TP_CHARGES)
		ShadowTPCharges = MAX_TP_CHARGES;
}

void UMyPlayerStats::RechargeTP(float deltaTime)
{
	MyUserWidget->FillShadowCharge((100 / TPRechargeTime)* deltaTime) ;
}


