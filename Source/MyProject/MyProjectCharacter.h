// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Logging/LogMacros.h"
#include "MyProjectCharacter.generated.h"

class UMyUserWidget;
class UMyPlayerStats;
class USoundCue;
class UInputComponent;
class USkeletalMeshComponent;
class UCameraComponent;
class UInputAction;
class UInputMappingContext;
struct FInputActionValue;

DECLARE_LOG_CATEGORY_EXTERN(LogTemplateCharacter, Log, All);

UCLASS(config=Game)
class AMyProjectCharacter : public ACharacter
{
	GENERATED_BODY()

	/** Pawn mesh: 1st person view (arms; seen only by self) */
	UPROPERTY(VisibleDefaultsOnly, Category=Mesh)
	USkeletalMeshComponent* Mesh1P;

	/** First person camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	UCameraComponent* FirstPersonCameraComponent;

	/** MappingContext */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Input, meta=(AllowPrivateAccess = "true"))
	UInputMappingContext* DefaultMappingContext;

	/** Jump Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Input, meta=(AllowPrivateAccess = "true"))
	UInputAction* JumpAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* CrouchAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* SprintAction;

	/** Move Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Input, meta=(AllowPrivateAccess = "true"))
	UInputAction* MoveAction;

	/** Look Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* LookAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* InteractAction;

	//temp tp
	class AShadowTP* CurrentTP;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Light, meta = (AllowPrivateAccess = "true"))
	class UPointLightComponent* CurrentPointLight;

	void ResetTP();
	void ResetPointLight();

public:
	AMyProjectCharacter();

    virtual void OnStartCrouch(float HalfHeightAdjust, float ScaledHalfHeightAdjust) override;
    virtual void OnEndCrouch(float HalfHeightAdjust, float ScaledHalfHeightAdjust) override;
    virtual void CalcCamera(float DeltaTime, FMinimalViewInfo& OutResult) override;

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Input)
	float MoveSpeed = 0.4f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Input)
	float SprintSpeed = 0.8f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement")
	bool isSprinting;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Raycast")
	float RayLength = 2000.f;

	//teleporting
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "TP")
	bool bCanTP;
	float StartingSpeed;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "TP")
	float TPDuration = 2.f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "TP")
	float TPThreshold = 300.f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "TP")
    TSubclassOf<UCameraShakeBase> TPCameraShake;
	bool bIsTping;
	float StartTime;
	FVector StartLocation;
	FHitResult hit;

	//light
	bool bCanExtinguishLight;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Fire")
	TSubclassOf<UCameraShakeBase> LightCameraShake;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Stats)
	UMyPlayerStats* MyPlayerStats;

	//temp bool, fix this later
	bool isWidgetSet;
public:

	/** Bool for AnimBP to switch to another animation set */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Weapon)
	bool bHasRifle;

	/** Setter to set the bool */
	UFUNCTION(BlueprintCallable, Category = Weapon)
	void SetHasRifle(bool bNewHasRifle);

	/** Getter for the bool */
	UFUNCTION(BlueprintCallable, Category = Weapon)
	bool GetHasRifle();

	void MyCrouch();

	void CastRayForInteraction();
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category= "Crouch")
	FVector CrouchEyeOffset;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Crouch")
	float CrouchSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Audio")
	UAudioComponent* FootstepAudioComponent;
	UPROPERTY(EditDefaultsOnly, Category = "Audio")
	USoundCue* WalkSoundCue;
	UPROPERTY(EditDefaultsOnly, Category = "Audio")
	USoundWave* TPSound;
	UPROPERTY(EditDefaultsOnly, Category = "Audio")
	USoundCue* LightExtinguishCue;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "UI")
	UMyUserWidget* MyUserWidget;

	void Sprint();
	void UnSprint();

	void Interact();


protected:
	/** Called for movement input */
	void Move(const FInputActionValue& Value);

	/** Called for looking input */
	void Look(const FInputActionValue& Value);

protected:
	// APawn interface
	virtual void SetupPlayerInputComponent(UInputComponent* InputComponent) override;
	// End of APawn interface

public:
	/** Returns Mesh1P subobject **/
	USkeletalMeshComponent* GetMesh1P() const { return Mesh1P; }
	/** Returns FirstPersonCameraComponent subobject **/
	UCameraComponent* GetFirstPersonCameraComponent() const { return FirstPersonCameraComponent; }

};

