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
	class UWidgetComponent* CurrentLightWidget;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Light, meta = (AllowPrivateAccess = "true"))
	class UPointLightComponent* CurrentPointLight;

	void ResetTP();
	void ResetPointLight();
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Light, meta = (AllowPrivateAccess = "true"))
	bool bIsPlayerInLight;
public:
	AMyProjectCharacter();

	/** Returns Mesh1P subobject **/
	USkeletalMeshComponent* GetMesh1P() const { return Mesh1P; }
	/** Returns FirstPersonCameraComponent subobject **/
	UCameraComponent* GetFirstPersonCameraComponent() const { return FirstPersonCameraComponent; }

    virtual void OnStartCrouch(float HalfHeightAdjust, float ScaledHalfHeightAdjust) override;
    virtual void OnEndCrouch(float HalfHeightAdjust, float ScaledHalfHeightAdjust) override;
    virtual void CalcCamera(float DeltaTime, FMinimalViewInfo& OutResult) override;

	void Sprint();
	void UnSprint();
	void MyCrouch();
	void CastRayForInteraction();
	void Interact();

	//npc ai
	void SpottedByNPC();
    void HeardByNPC();
	bool IsPlayerInLight() const;
	UFUNCTION(BlueprintCallable, Category = "Player")
	void SetIsPlayerInLight(bool b);
	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, Category = "Player")
	void ToggleVisibilityBlocker();


	UFUNCTION(BlueprintImplementableEvent, Category ="GameOver")
	void GameEnd();

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Crouch")
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

	

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Input)
	float MoveSpeed = 0.4f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Input)
	float SprintSpeed = 0.8f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Movement)
	float Loudness = 1.1f;
	FName TagForSound = TEXT("Player Footsteps");

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
	FHitResult Hit;

	//light
	bool bCanExtinguishLight;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Fire")
	TSubclassOf<UCameraShakeBase> LightCameraShake;
	//stats
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Stats)
	UMyPlayerStats* MyPlayerStats;

	/** Called for movement input */
	void Move(const FInputActionValue& Value);
	/** Called for looking input */
	void Look(const FInputActionValue& Value);

	virtual void SetupPlayerInputComponent(UInputComponent* InputComponent) override;

	//temp bool, fix this later
	bool bIsWidgetSet;
	bool bIsGameOver;

private:

};

