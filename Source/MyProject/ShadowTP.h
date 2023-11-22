// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ShadowTP.generated.h"

UCLASS()
class MYPROJECT_API AShadowTP : public AActor
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly,BlueprintReadWrite, Category = TP,meta = (AllowPrivateAccess = "true"));
	bool bCanBeUsed;
	bool bIsRaycasting;
	UPROPERTY(VisibleAnywhere, Category=TP);
	class UWidgetComponent* WidgetComponent;

	FVector ActorLocation;
public:	
	// Sets default values for this actor's properties
	AShadowTP();

	UFUNCTION(BlueprintCallable, Category = TP)
	bool CanBeUsed() const;
	UFUNCTION(BlueprintCallable, Category = TP)
	void DisableTP();
	UFUNCTION(BlueprintCallable, Category = TP)
	void EnableTP();
    void isRayCasting(bool b, const FVector& ActorLocation);
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
