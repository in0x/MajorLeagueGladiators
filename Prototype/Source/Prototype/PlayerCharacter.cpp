// Fill out your copyright notice in the Description page of Project Settings.

#include "Prototype.h"
#include "PlayerCharacter.h"
#include "IPlayerCharacterMotionController.h"
#include "VRControllerComponent.h"
#include <algorithm>

APlayerCharacter::APlayerCharacter(const FObjectInitializer& ObjectInitializer /*= FObjectInitializer::Get()*/)
	: Super(ObjectInitializer
		.SetDefaultSubobjectClass<UVRControllerComponent>(TEXT("Left Grip Motion Controller"))
		.SetDefaultSubobjectClass<UVRControllerComponent>(TEXT("Right Grip Motion Controller"))
	)
{
	bUseControllerRotationPitch = true;

	leftMesh = ObjectInitializer.CreateDefaultSubobject<UStaticMeshComponent>(this, TEXT("LeftMesh"));
	rightMesh = ObjectInitializer.CreateDefaultSubobject<UStaticMeshComponent>(this, TEXT("RightMesh"));

	leftMesh->SetupAttachment(LeftMotionController);
	rightMesh->SetupAttachment(RightMotionController);

	leftGrabSphere = ObjectInitializer.CreateDefaultSubobject<USphereComponent>(this, TEXT("LeftGrabSphere"));
	rightGrabSphere = ObjectInitializer.CreateDefaultSubobject<USphereComponent>(this, TEXT("RightGrabSphere"));

	leftGrabSphere->SetupAttachment(LeftMotionController);
	rightGrabSphere->SetupAttachment(RightMotionController);

	// ArrowComponent is used to determine position and direction for teleport.
	vrArrow = CastChecked<UArrowComponent>(GetComponentByClass(UArrowComponent::StaticClass())); 
}

void APlayerCharacter::BeginPlay()
{
	Super::BeginPlay();

	// If no HMD is connected, setup non VR mode.
	if (!GEngine->HMDDevice.IsValid() || !GEngine->HMDDevice->IsHMDConnected()) 
	{
		pHandMotionController = std::make_unique<HandMotionController>(this);

		// Set Capsule collider dimensions so that the player has an actual height in non VR mode.
		// Having this set in VR Mode causes the HMD to be offset too far from the floor.
		auto capsule = CastChecked<UCapsuleComponent>(GetRootComponent());
		capsule->SetCapsuleRadius(20.f);
		capsule->SetCapsuleHalfHeight(96.f);

		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, TEXT("NON VR MODE"));
	}	
	else
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow, TEXT("VR MODE"));
	}
}

void APlayerCharacter::Tick(float DeltaTime)
{	
}

void APlayerCharacter::SetupPlayerInputComponent(UInputComponent* playerInputComponent)
{
	Super::SetupPlayerInputComponent(playerInputComponent);

	playerInputComponent->BindAxis("MoveForward", this, &APlayerCharacter::MoveForward);
	playerInputComponent->BindAxis("MoveRight", this, &APlayerCharacter::MoveRight);

	playerInputComponent->BindAxis("Turn", this, &APlayerCharacter::AddControllerYawInput);
	playerInputComponent->BindAxis("LookUp", this, &APlayerCharacter::AddControllerPitchInput);

	playerInputComponent->BindAxis("LeftTrigger", this, &APlayerCharacter::OnLeftTriggerAxis);
	playerInputComponent->BindAxis("RightTrigger", this, &APlayerCharacter::OnRightTriggerAxis);

	playerInputComponent->BindAction("LeftTriggerClicked", EInputEvent::IE_Pressed,  this, &APlayerCharacter::OnLeftTriggerClicked);
	playerInputComponent->BindAction("LeftTriggerClicked", EInputEvent::IE_Released, this, &APlayerCharacter::OnLeftTriggerReleased);

	playerInputComponent->BindAction("RightTriggerClicked", EInputEvent::IE_Pressed,  this, &APlayerCharacter::OnRightTriggerClicked);
	playerInputComponent->BindAction("RightTriggerClicked", EInputEvent::IE_Released, this, &APlayerCharacter::OnRightTriggerReleased);

	playerInputComponent->BindAction("TeleportPress", EInputEvent::IE_Pressed, this, &APlayerCharacter::OnTeleportPressed);
	playerInputComponent->BindAction("TeleportPress", EInputEvent::IE_Released, this, &APlayerCharacter::OnTeleportReleased);
}

void APlayerCharacter::MoveForward(float value)
{
	FVector direction = FRotationMatrix(Controller->GetControlRotation()).GetScaledAxis(EAxis::X);
	AddMovementInput(direction, value);
}

void APlayerCharacter::MoveRight(float value)
{
	FVector direction = FRotationMatrix(Controller->GetControlRotation()).GetScaledAxis(EAxis::Y);
	AddMovementInput(direction, value);
}

void APlayerCharacter::OnLeftTriggerAxis(float value)
{
	if (value > 0.f)
	{
		GEngine->AddOnScreenDebugMessage(-1, 0.5f, FColor::Green, FString::Printf(TEXT("LeftTrigger: %f"), value), false);
	}
}

void APlayerCharacter::OnRightTriggerAxis(float value)
{
	if (value > 0.f)
	{
		GEngine->AddOnScreenDebugMessage(-1, 0.5f, FColor::Green, FString::Printf(TEXT("RightTrigger: %f"), value), false);
	}
}

void APlayerCharacter::OnLeftTriggerClicked()
{
	CastChecked<UVRControllerComponent>(LeftMotionController)->GrabNearestActor(*leftGrabSphere);
}

void APlayerCharacter::OnLeftTriggerReleased()
{
	CastChecked<UVRControllerComponent>(LeftMotionController)->DropAllGrips();
}

void APlayerCharacter::OnRightTriggerClicked()
{
	CastChecked<UVRControllerComponent>(RightMotionController)->GrabNearestActor(*rightGrabSphere);
}

void APlayerCharacter::OnRightTriggerReleased()
{
	CastChecked<UVRControllerComponent>(RightMotionController)->DropAllGrips();
}

void APlayerCharacter::OnTeleportPressed()
{
	GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Yellow, TEXT("OnTeleportPressed"));
	
	//FHitResult result;
	//TArray<FVector> positions;
	//FVector lastTraceDest;
	///*FVector vel(100, 0, 50);*/
	//TArray<TEnumAsByte<EObjectTypeQuery>> queryTypes{ EObjectTypeQuery::ObjectTypeQuery1 };

	//UGameplayStatics::PredictProjectilePath(GetWorld(),
	//	result,
	//	positions,
	//	lastTraceDest,
	//	GetActorLocation(),
	//	GetActorForwardVector() * 100, //vel,
	//	true,
	//	10,
	//	queryTypes,
	//	true,
	//	TArray<AActor*>(),
	//	EDrawDebugTrace::ForOneFrame,
	//	1.f);

	FHitResult result;
	TArray<FVector> positions;
	FVector lastTraceDest;
	TArray<TEnumAsByte<EObjectTypeQuery>> queryTypes{ EObjectTypeQuery::ObjectTypeQuery1 };

	UGameplayStatics::PredictProjectilePath(
		GetWorld(),
		result,
		positions,
		lastTraceDest,
		vrArrow->GetComponentLocation(),
		vrArrow->GetForwardVector() * 900,
		true,
		0, 
		queryTypes,
		false,
		TArray<AActor*>(),
		EDrawDebugTrace::ForDuration,
		5.f
	);

	
}

void APlayerCharacter::OnTeleportReleased()
{
	GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Yellow, TEXT("OnTeleportReleased"));
}