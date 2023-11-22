// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include <vector>
#include "MyUserWidget.generated.h"
/**
 * 
 */
class UProgressBar;
UCLASS()	
class MYPROJECT_API UMyUserWidget : public UUserWidget
{
	GENERATED_BODY()
	UProgressBar* CurrentShadowFilling;
public:

	UMyUserWidget(const FObjectInitializer& ObjectInitializer);
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = UI, meta = (BindWidget));
	TArray<UProgressBar*> ShadowCharges;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = UI, meta = (BindWidget));
	class UImage* EffectImage;

	void ShadowChargedRefilled();
	void FillShadowCharge(float value);
	void UseShadowCharge();
	UFUNCTION(BlueprintCallable, Category=UI)
	void EffectVisibility(bool b);

	UFUNCTION(BlueprintCallable, Category = UI)
	class UImage* GetImage();

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = UI);
	bool bIsEffectVisible;
protected:
};
