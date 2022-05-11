// Copyright Epic Games, Inc. All Rights Reserved.

#include "MyProjectCharacter.h"
#include "HeadMountedDisplayFunctionLibrary.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/SpringArmComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Net/UnrealNetwork.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "MyPlayerController.h"

void AMyProjectCharacter::Server_PlayHUH_Implementation()
{
	Multicast_PlayHUH();
}

void AMyProjectCharacter::Multicast_PlayHUH_Implementation()
{
	if (HUHMontage)
	{
		PlayAnimMontage(HUHMontage);
	}
}

FHitResult AMyProjectCharacter::ThrowRaycast()
{
	const FVector& CameraLocation = FollowCamera->GetComponentLocation();
	const FRotator& CameraRotation = FollowCamera->GetComponentRotation();
	FVector EndLocation = CameraLocation + (CameraRotation.Vector() * 1000.f);
	FHitResult HitResult;
	const bool bIsHitted = UKismetSystemLibrary::LineTraceSingle(GetWorld(), CameraLocation, EndLocation, 
		ETraceTypeQuery::TraceTypeQuery_MAX, true, {this}, EDrawDebugTrace::ForDuration, HitResult, true);

	return HitResult;
}

//////////////////////////////////////////////////////////////////////////
// AMyProjectCharacter

AMyProjectCharacter::AMyProjectCharacter()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	// set our turn rates for input
	BaseTurnRate = 45.f;
	BaseLookUpRate = 45.f;

	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...	
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 540.0f, 0.0f); // ...at this rotation rate
	GetCharacterMovement()->JumpZVelocity = 600.f;
	GetCharacterMovement()->AirControl = 0.2f;

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 300.0f; // The camera follows at this distance behind the character	
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character) 
	// are set in the derived blueprint asset named MyCharacter (to avoid direct content references in C++)
}

void AMyProjectCharacter::Die_Implementation()
{
	USkeletalMeshComponent* SkeletalMesh = GetMesh();
	UCapsuleComponent* MyCapsule = GetCapsuleComponent();
	UCharacterMovementComponent* MyMovement = GetCharacterMovement();

	if (SkeletalMesh)
	{
		SkeletalMesh->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);
		SkeletalMesh->SetCollisionObjectType(ECollisionChannel::ECC_PhysicsBody);
		SkeletalMesh->SetSimulatePhysics(true);
	}

	if (MyCapsule)
	{
		MyCapsule->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}

	if (MyMovement)
	{
		MyMovement->DisableMovement();
	}

	if (HasAuthority() && !RespawnTimer.IsValid())
	{
		GetWorldTimerManager().SetTimer(RespawnTimer, this, &AMyProjectCharacter::OnRespawn, 2.f, false);
	}
}

void AMyProjectCharacter::OnRespawn_Implementation()
{
	AMyPlayerController* MyPlayerController = Cast<AMyPlayerController>(GetController());
	if (MyPlayerController)
	{
		MyPlayerController->UnPossess();
		MyPlayerController->RespawnPawn(InitTransform);
	}
}

//////////////////////////////////////////////////////////////////////////
// Input

void AMyProjectCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// Set up gameplay key bindings
	check(PlayerInputComponent);
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);
	PlayerInputComponent->BindAction("Fire", IE_Pressed, this, &AMyProjectCharacter::FireWeapon);

	PlayerInputComponent->BindAxis("MoveForward", this, &AMyProjectCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &AMyProjectCharacter::MoveRight);

	// We have 2 versions of the rotation bindings to handle different kinds of devices differently
	// "turn" handles devices that provide an absolute delta, such as a mouse.
	// "turnrate" is for devices that we choose to treat as a rate of change, such as an analog joystick
	PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("TurnRate", this, &AMyProjectCharacter::TurnAtRate);
	PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("LookUpRate", this, &AMyProjectCharacter::LookUpAtRate);

	// handle touch devices
	PlayerInputComponent->BindTouch(IE_Pressed, this, &AMyProjectCharacter::TouchStarted);
	PlayerInputComponent->BindTouch(IE_Released, this, &AMyProjectCharacter::TouchStopped);

	// VR headset functionality
	PlayerInputComponent->BindAction("ResetVR", IE_Pressed, this, &AMyProjectCharacter::OnResetVR);
}

void AMyProjectCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AMyProjectCharacter, CurrentHealth);
}

void AMyProjectCharacter::BeginPlay()
{
	Super::BeginPlay();
	CurrentHealth = MaxHealth;
	InitTransform = GetActorTransform();
}

void AMyProjectCharacter::OnResetVR()
{
	// If MyProject is added to a project via 'Add Feature' in the Unreal Editor the dependency on HeadMountedDisplay in MyProject.Build.cs is not automatically propagated
	// and a linker error will result.
	// You will need to either:
	//		Add "HeadMountedDisplay" to [YourProject].Build.cs PublicDependencyModuleNames in order to build successfully (appropriate if supporting VR).
	// or:
	//		Comment or delete the call to ResetOrientationAndPosition below (appropriate if not supporting VR)
	UHeadMountedDisplayFunctionLibrary::ResetOrientationAndPosition();
}

void AMyProjectCharacter::TouchStarted(ETouchIndex::Type FingerIndex, FVector Location)
{
		Jump();
}

void AMyProjectCharacter::TouchStopped(ETouchIndex::Type FingerIndex, FVector Location)
{
		StopJumping();
}

void AMyProjectCharacter::FireWeapon_Implementation()
{
	//Server_PlayHUH();
	FHitResult HittedResult = ThrowRaycast();
	if (!HittedResult.IsValidBlockingHit())
		return;

	AActor* HittedActor = HittedResult.GetActor();
	if (HittedActor)
	{
		const FVector& ImpactPoint = HittedResult.ImpactPoint;
		const FVector& TraceStart = HittedResult.TraceStart;
		const FVector& HitFromDirection = UKismetMathLibrary::GetDirectionUnitVector(ImpactPoint, TraceStart);
		UGameplayStatics::ApplyPointDamage(HittedActor, UKismetMathLibrary::RandomFloatInRange(10.f, 20.f), HitFromDirection, HittedResult, 
			GetController(), this, UDamageType::StaticClass());
	}
}

void AMyProjectCharacter::TurnAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}

void AMyProjectCharacter::LookUpAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
}

void AMyProjectCharacter::MoveForward(float Value)
{
	if ((Controller != nullptr) && (Value != 0.0f))
	{
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		AddMovementInput(Direction, Value);
	}
}

void AMyProjectCharacter::MoveRight(float Value)
{
	if ( (Controller != nullptr) && (Value != 0.0f) )
	{
		// find out which way is right
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);
	
		// get right vector 
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		// add movement in that direction
		AddMovementInput(Direction, Value);
	}
}
