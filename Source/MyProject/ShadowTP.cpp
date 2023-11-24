// Fill out your copyright notice in the Description page of Project Settings.


#include "ShadowTP.h"

#include "Components/Image.h"
#include "Components/WidgetComponent.h"
#include "Blueprint/UserWidget.h"
#include "Kismet/KismetMathLibrary.h"

// Sets default values
AShadowTP::AShadowTP()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	bCanBeUsed = true;
	
}

// Called when the game starts or when spawned
void AShadowTP::BeginPlay()
{
	Super::BeginPlay();
	WidgetComponent = FindComponentByClass<UWidgetComponent>();
}

// Called every frame
void AShadowTP::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if(!bIsRaycasting && WidgetComponent)
	{
		if (!WidgetComponent->IsVisible())
			return;
		WidgetComponent->SetVisibility(false);
		bCanBeUsed = false;
	}
	else if(bIsRaycasting && bCanBeUsed && WidgetComponent)
	{
		FVector Direction = GetActorLocation() - WidgetComponent->GetComponentLocation();
		FRotator Rot = UKismetMathLibrary::FindLookAtRotation(WidgetComponent->GetComponentLocation(), ActorLocation);
		WidgetComponent->SetWorldRotation(Rot);
	    if(WidgetComponent->IsVisible())
	        return;
	    WidgetComponent->SetVisibility(true);
	}
}

bool AShadowTP::CanBeUsed() const
{
	return bCanBeUsed;
}

void AShadowTP::DisableTP()
{
    bIsRaycasting = false;
	if (WidgetComponent)
	{
		WidgetComponent->SetVisibility(false);
	}
}

void AShadowTP::EnableTP()
{
	bCanBeUsed = true;
}

void AShadowTP::isRayCasting(bool b, const FVector& actorLocation)
{
	bIsRaycasting = b;
	ActorLocation = actorLocation;
}

