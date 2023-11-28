// Copyright Epic Games, Inc. All Rights Reserved.

#include "MyProjectCharacter.h"
#include "MyProjectProjectile.h"
#include "Animation/AnimInstance.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "FrameTypes.h"
#include "InputActionValue.h"
#include "MyPlayerStats.h"
#include "MyUserWidget.h"
#include "ShadowTP.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Engine/LocalPlayer.h"
#include "Kismet/GameplayStatics.h"
#include "Components/AudioComponent.h"
#include "Components/PointLightComponent.h"
#include "Components/WidgetComponent.h"
#include "Kismet/KismetMathLibrary.h"

#include "Perception/AIPerceptionStimuliSourceComponent.h"
#include "Perception/AIPerceptionSystem.h"
#include "Perception/AISense_Hearing.h"
#include "Perception/AISense_Sight.h"

#include "Sound/SoundCue.h"
DEFINE_LOG_CATEGORY(LogTemplateCharacter);

//////////////////////////////////////////////////////////////////////////
// AMyProjectCharacter

AMyProjectCharacter::AMyProjectCharacter()
{
	StartingSpeed = MoveSpeed;
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(55.f, 96.0f);
		
	// Create a CameraComponent	
	FirstPersonCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
	FirstPersonCameraComponent->SetupAttachment(GetCapsuleComponent());
	FirstPersonCameraComponent->SetRelativeLocation(FVector(-10.f, 0.f, 60.f)); // Position the camera
	FirstPersonCameraComponent->bUsePawnControlRotation = true;

	// Create a mesh component that will be used when being viewed from a '1st person' view (when controlling this pawn)
	Mesh1P = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("CharacterMesh1P"));
	Mesh1P->SetOnlyOwnerSee(true);
	Mesh1P->SetupAttachment(FirstPersonCameraComponent);
	Mesh1P->bCastDynamicShadow = false;
	Mesh1P->CastShadow = false;
	//Mesh1P->SetRelativeRotation(FRotator(0.9f, -19.19f, 5.2f));
	Mesh1P->SetRelativeLocation(FVector(-30.f, 0.f, -150.f));

	//stats
	MyPlayerStats = CreateDefaultSubobject<UMyPlayerStats>(TEXT("MyStats"));

	//crouch setup
	CrouchEyeOffset = FVector::ZeroVector;
	CrouchSpeed = 6.f;
	//footsteps audio setup
	FootstepAudioComponent = CreateDefaultSubobject<UAudioComponent>(TEXT("FootstepAudioComponent"));
	FootstepAudioComponent->SetupAttachment(RootComponent);
	FootstepAudioComponent->bAutoActivate = false;
	FootstepAudioComponent->VolumeMultiplier = 0.7f;
}

void AMyProjectCharacter::BeginPlay()
{
	// Call the base class  
	Super::BeginPlay();
	// Add Input Mapping Context
	if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		};
	}
	if(MyUserWidget && MyPlayerStats)
	{
		//MyUserWidget->SetShadowCharge(MyPlayerStats->GetCharges());
	}

	//ai

	CurrentTP = nullptr;
	CurrentPointLight = nullptr;
	CurrentLightWidget = nullptr;
	bIsGameOver = false;
}

void AMyProjectCharacter::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);
	if (bIsGameOver)
	{
		FootstepAudioComponent->Stop();
		return;
	}
	if (MyUserWidget && !bIsWidgetSet)
	{
		bIsWidgetSet = true;
		UE_LOG(LogTemp, Warning, TEXT("setting user widget"));
		MyPlayerStats->SetUserWidget(MyUserWidget);
	}
	//crouch
	float CrouchInterpTime = FMath::Min(1.f, CrouchSpeed * DeltaSeconds);
	CrouchEyeOffset = (1.f - CrouchInterpTime) * CrouchEyeOffset;
	//footstep stop
	if((GetVelocity().X==0 && GetVelocity().Y==0) || bIsCrouched)
	{
		//UE_LOG(LogTemp, Warning, TEXT("STOPPING"));
		FootstepAudioComponent->Stop();
	}

	//interaction raycast
	CastRayForInteraction();

	//tp logic(sliding)
	if(bIsTping)
	{
		float Alpha = (GetWorld()->GetTimeSeconds() - StartTime) / TPDuration;
		Alpha = FMath::Clamp(Alpha, 0.0f, 1.0f);
		FVector NewLocation = FMath::Lerp(StartLocation, Hit.GetActor()->GetActorLocation(), Alpha);
		SetActorLocation(NewLocation);
		//if(Alpha>=0.8f)
		if (FVector::Distance(GetActorLocation(), Hit.GetActor()->GetActorLocation())<TPThreshold)
		{
			bIsTping = false;
		}
	}
	if(bCanExtinguishLight)
	{
		FVector Direction = GetActorLocation() - CurrentLightWidget->GetComponentLocation();
		FRotator Rot = UKismetMathLibrary::FindLookAtRotation(CurrentLightWidget->GetComponentLocation(), GetActorLocation());
		CurrentLightWidget->SetWorldRotation(Rot);
	}
}

//////////////////////////////////////////////////////////////////////////// Input


void AMyProjectCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		// Jumping
		//EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &ACharacter::Jump);
		//EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);

		// Moving
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AMyProjectCharacter::Move);

		// Looking
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &AMyProjectCharacter::Look);

		// Crouching
		EnhancedInputComponent->BindAction(CrouchAction, ETriggerEvent::Started, this, &AMyProjectCharacter::MyCrouch);

		//Sprinting
		EnhancedInputComponent->BindAction(SprintAction, ETriggerEvent::Triggered, this, &AMyProjectCharacter::Sprint);
		EnhancedInputComponent->BindAction(SprintAction, ETriggerEvent::Completed, this, &AMyProjectCharacter::UnSprint);

		//TP
		EnhancedInputComponent->BindAction(InteractAction, ETriggerEvent::Started, this, &AMyProjectCharacter::Interact);

	}
	else
	{
		UE_LOG(LogTemplateCharacter, Error, TEXT("'%s' Failed to find an Enhanced Input Component! This template is built to use the Enhanced Input system. If you intend to use the legacy system, then you will need to update this C++ file."), *GetNameSafe(this));
	}
}



void AMyProjectCharacter::Move(const FInputActionValue& Value)
{
	if (bIsGameOver)
		return;
	// input is a Vector2D
	FVector2D MovementVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// add movement 
		AddMovementInput(GetActorForwardVector(), MovementVector.Y * MoveSpeed);
		AddMovementInput(GetActorRightVector(), MovementVector.X * MoveSpeed);
	    if (MovementVector.Y != 0.0f || MovementVector.X != 0.0f)
		{
			FootstepAudioComponent->Sound = WalkSoundCue;
			if (isSprinting)
			{
				//UE_LOG(LogTemp, Warning, TEXT("SPRINGING"));
				FootstepAudioComponent->SetPitchMultiplier(SprintSpeed*2.45f);
				Loudness = 1.75f;
			}
			else if (WalkSoundCue)
			{
				//UE_LOG(LogTemp, Warning, TEXT("WALKING"));
				FootstepAudioComponent->SetPitchMultiplier(MoveSpeed*2.45f); // Reset pitch for walking
				Loudness = 1.1f;
			}

			if (!FootstepAudioComponent->IsPlaying())
			{
				//UE_LOG(LogTemp, Warning, TEXT("PLAYING"));
				FootstepAudioComponent->Play();
			}
			if(!bIsCrouched)
			    UAISense_Hearing::ReportNoiseEvent(GetWorld(), GetActorLocation(), Loudness, this, 3000.f, TagForSound);
		}
	}
}

void AMyProjectCharacter::Look(const FInputActionValue& Value)
{
	if (bIsGameOver)
		return;
	// input is a Vector2D
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// add yaw and pitch input to controller
		AddControllerYawInput(LookAxisVector.X);
		AddControllerPitchInput(LookAxisVector.Y);
	}
}

void AMyProjectCharacter::OnStartCrouch(float HalfHeightAdjust, float ScaledHalfHeightAdjust)
{
	if (HalfHeightAdjust == 0.0f)
		return;

	float const StartBaseEyeHeight = BaseEyeHeight;
	Super::OnStartCrouch(HalfHeightAdjust, ScaledHalfHeightAdjust);
	CrouchEyeOffset.Z = StartBaseEyeHeight - BaseEyeHeight + HalfHeightAdjust;
	FirstPersonCameraComponent->SetRelativeLocation(FVector(0.f, 0.f, BaseEyeHeight), false);
}

void AMyProjectCharacter::OnEndCrouch(float HalfHeightAdjust, float ScaledHalfHeightAdjust)
{
	if (HalfHeightAdjust == 0.0f)
		return;
	float const StartBaseEyeHeight = BaseEyeHeight;
	Super::OnEndCrouch(HalfHeightAdjust, ScaledHalfHeightAdjust);
	CrouchEyeOffset.Z = StartBaseEyeHeight - BaseEyeHeight - HalfHeightAdjust;
	FirstPersonCameraComponent->SetRelativeLocation(FVector(0.f, 0.f, BaseEyeHeight), false);
}

void AMyProjectCharacter::CalcCamera(float DeltaTime, FMinimalViewInfo& OutResult)
{
	//Super::CalcCamera(DeltaTime, OutResult);
	if (FirstPersonCameraComponent)
	{
		FirstPersonCameraComponent->GetCameraView(DeltaTime, OutResult);
		OutResult.Location += CrouchEyeOffset;
	}
}

void AMyProjectCharacter::MyCrouch()
{
	if (bIsGameOver)
		return;
	if(!bIsCrouched)
	    Crouch();
	else
		UnCrouch();
}

void AMyProjectCharacter::Sprint()
{
	if (bIsCrouched)
		UnCrouch();
	MoveSpeed = SprintSpeed;
	isSprinting = true;
	//UE_LOG(LogTemp, Warning, TEXT("sprinting %f"), MoveSpeed);
}

void AMyProjectCharacter::UnSprint()
{
    MoveSpeed = StartingSpeed;
	isSprinting = false;
	//UE_LOG(LogTemp, Warning, TEXT("not sprinting %f"), MoveSpeed);
}

void AMyProjectCharacter::CastRayForInteraction()
{
	if (bIsTping)
	{
		bCanTP = false;
		return;
	}
	FVector Start = GetActorLocation() + FVector(0, 0, 10.f);
	FVector Dir = FirstPersonCameraComponent->GetForwardVector();
	Start = FVector(Start.X + (Dir.X * 100), Start.Y + (Dir.Y * 100), Start.Z + (Dir.Z * 150));
	FVector End = Start + (Dir * RayLength);
	bool ActorHit = GetWorld()->LineTraceSingleByChannel(Hit, Start, End, ECC_GameTraceChannel1, FCollisionQueryParams(), FCollisionResponseParams());

	DrawDebugLine(GetWorld(), Start, End, FColor::Red, false, 0.1f, 0.f, 5.f);
	//if(hit.GetActor())
	    //UE_LOG(LogTemp, Warning, TEXT("obj %s"), *hit.GetActor()->GetActorNameOrLabel());
	//raycasting shadow tp
	if(!ActorHit)
	{
		ResetTP();
		ResetPointLight();
	}
	else if (ActorHit && Hit.GetActor()->IsA<AShadowTP>())
	{
		ResetPointLight();
		CurrentTP = Cast<AShadowTP>(Hit.GetActor());
		if (!CurrentTP->CanBeUsed())
		{
			bCanTP = false;
		}
		else
		{
			bCanTP = true;
			CurrentTP->EnableTP();
			CurrentTP->isRayCasting(true, GetActorLocation());
		}
	}
	//raycasting point light
	else if (ActorHit && Hit.GetActor()->GetComponentByClass<UPointLightComponent>())
	{
		ResetTP();
		if (!Hit.GetActor()->GetActorEnableCollision())
			return;
		if (Hit.GetActor()->GetComponentByClass<UWidgetComponent>())
		{
			CurrentLightWidget = Hit.GetActor()->GetComponentByClass<UWidgetComponent>();
			CurrentLightWidget->SetVisibility(true);
		}
		else
			return;
		CurrentPointLight = (Hit.GetActor()->GetComponentByClass<UPointLightComponent>());
		bCanExtinguishLight = true;
	}
}

void AMyProjectCharacter::ResetTP()
{
	bCanTP = false;
	if (CurrentTP)
	{
		CurrentTP->DisableTP();
		CurrentTP->isRayCasting(false, FVector::Zero());
		CurrentTP = nullptr;
	}
}

void AMyProjectCharacter::ResetPointLight()
{
	bCanExtinguishLight = false;
	if (CurrentPointLight)
	{
		CurrentLightWidget->SetVisibility(false);
		CurrentLightWidget = nullptr;
		CurrentPointLight = nullptr;
	}
}
void AMyProjectCharacter::Interact()
{
	if (bIsGameOver)
		return;
    if (!MyPlayerStats || MyPlayerStats->GetCharges() <= 0)
			return; //early return
	
	if (bCanTP)
	{
		MyPlayerStats->UseCharge();
		bIsTping = true;
		if(CurrentTP)
		{
			CurrentTP->DisableTP();
		}
		//play sound
		if(TPSound)
		    UGameplayStatics::PlaySound2D(this, TPSound);
		bCanTP = false;
		//camera shake
		StartTime = GetWorld()->GetTimeSeconds();
	    StartLocation = GetActorLocation();
		UGameplayStatics::PlayWorldCameraShake(this, TPCameraShake, GetActorLocation(), 0, 500);
	}
	else if(bCanExtinguishLight)
	{
		MyPlayerStats->UseCharge();
		bCanExtinguishLight = false;
		UE_LOG(LogTemp, Warning, TEXT("light extinguished"));

		UGameplayStatics::PlayWorldCameraShake(this, LightCameraShake, GetActorLocation(), 0, 500);
		if (LightExtinguishCue)
			UGameplayStatics::PlaySound2D(this, LightExtinguishCue);
		if (bIsPlayerInLight)
			SetIsPlayerInLight(false);
		UE_LOG(LogTemp, Warning, TEXT("in light : %hhd"), bIsPlayerInLight);
		CurrentPointLight->SetIntensity(0.f);
		CurrentPointLight->GetOwner()->SetActorTickEnabled(false);
		CurrentPointLight->GetOwner()->SetActorEnableCollision(false);
		ResetPointLight();
	}
}

void AMyProjectCharacter::SpottedByNPC()
{
    bIsGameOver = true;
    GameEnd();
}

void AMyProjectCharacter::HeardByNPC()
{
	//UE_LOG(LogTemp, Warning, TEXT("heard by npc"));
}

bool AMyProjectCharacter::IsPlayerInLight() const
{
	return bIsPlayerInLight;
}

void AMyProjectCharacter::SetIsPlayerInLight(bool b)
{
	UE_LOG(LogTemp, Warning, TEXT("in light : %hhd"),b);
	bIsPlayerInLight = b;
	ToggleVisibilityBlocker();
}
