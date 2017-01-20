// Fill out your copyright notice in the Description page of Project Settings.

#include "Prototype.h"
#include "PlayerCharacter.h"
#include "VRControllerComponent.h"
#include "TeleportComponent.h"
#include "HealthComponent.h"
#include "PrototypePlayerController.h"
#include "PlayerHudWidget.h"

APlayerCharacter::APlayerCharacter(const FObjectInitializer& ObjectInitializer /*= FObjectInitializer::Get()*/)
	: Super(ObjectInitializer
		.SetDefaultSubobjectClass<UVRControllerComponent>(TEXT("Left Grip Motion Controller"))
		.SetDefaultSubobjectClass<UVRControllerComponent>(TEXT("Right Grip Motion Controller"))
	)
{
	bUseControllerRotationPitch = true;

	healthComp = ObjectInitializer.CreateDefaultSubobject<UHealthComponent>(this, "HealthComp");
	healthComp->SetIsReplicated(true);

	leftMesh = ObjectInitializer.CreateDefaultSubobject<UStaticMeshComponent>(this, TEXT("LeftMesh"));
	rightMesh = ObjectInitializer.CreateDefaultSubobject<UStaticMeshComponent>(this, TEXT("RightMesh"));

	leftMesh->SetupAttachment(LeftMotionController);
	rightMesh->SetupAttachment(RightMotionController);

	leftGrabSphere = ObjectInitializer.CreateDefaultSubobject<USphereComponent>(this, TEXT("LeftGrabSphere"));
	rightGrabSphere = ObjectInitializer.CreateDefaultSubobject<USphereComponent>(this, TEXT("RightGrabSphere"));

	leftGrabSphere->SetupAttachment(LeftMotionController);
	rightGrabSphere->SetupAttachment(RightMotionController);

	teleportComp = ObjectInitializer.CreateDefaultSubobject<UTeleportComponent>(this, TEXT("TeleportComp"));
	teleportComp->Disable();
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

void APlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis("MoveForward", this, &APlayerCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &APlayerCharacter::MoveRight);

	PlayerInputComponent->BindAxis("Turn", this, &APlayerCharacter::AddControllerYawInput);
	PlayerInputComponent->BindAxis("LookUp", this, &APlayerCharacter::AddControllerPitchInput);

	PlayerInputComponent->BindAction("LeftTriggerClicked", EInputEvent::IE_Pressed,  this, &APlayerCharacter::OnLeftTriggerClicked);
	PlayerInputComponent->BindAction("LeftTriggerClicked", EInputEvent::IE_Released, this, &APlayerCharacter::OnLeftTriggerReleased);

	PlayerInputComponent->BindAction("RightTriggerClicked", EInputEvent::IE_Pressed,  this, &APlayerCharacter::OnRightTriggerClicked);
	PlayerInputComponent->BindAction("RightTriggerClicked", EInputEvent::IE_Released, this, &APlayerCharacter::OnRightTriggerReleased);

	PlayerInputComponent->BindAction("TeleportPressLeft", EInputEvent::IE_Pressed, this, &APlayerCharacter::OnTeleportPressedLeft);
	PlayerInputComponent->BindAction("TeleportPressRight", EInputEvent::IE_Pressed, this, &APlayerCharacter::OnTeleportPressedRight);

	PlayerInputComponent->BindAction("TeleportPressLeft", EInputEvent::IE_Released, this, &APlayerCharacter::OnTeleportReleased);
	PlayerInputComponent->BindAction("TeleportPressRight", EInputEvent::IE_Released, this, &APlayerCharacter::OnTeleportReleased);

	PlayerInputComponent->BindAction("SideGripButtonLeft", EInputEvent::IE_Pressed, this, &APlayerCharacter::OnSideGripButtonLeft);
	PlayerInputComponent->BindAction("SideGripButtonRight", EInputEvent::IE_Pressed, this, &APlayerCharacter::OnSideGripButtonRight);
}

void APlayerCharacter::BecomeViewTarget(APlayerController* PC) {
	//NOTE: we might want to change this function to a onPosess function instead (if Actors change during game)
	Super::BecomeViewTarget(PC);
	//checks if the controller that became a viewController is my local controller, and add the HUD widget to this controller
	if (PC == UGameplayStatics::GetPlayerController(GetWorld(), 0)) {
		APrototypePlayerController* prototypePlayerController = CastChecked<APrototypePlayerController>(PC);
		prototypePlayerController->InitHudWidget();
		UPlayerHudWidget* currentWidget = prototypePlayerController->GetHudWidget();
		if (currentWidget != nullptr) 
		{
			currentWidget->OnAttachPlayer(this);
		}
	}
}



void APlayerCharacter::MoveForward(float Value)
{
	FVector direction = FRotationMatrix(Controller->GetControlRotation()).GetScaledAxis(EAxis::X);
	AddMovementInput(direction, Value);
}

void APlayerCharacter::MoveRight(float Value)
{
	FVector direction = FRotationMatrix(Controller->GetControlRotation()).GetScaledAxis(EAxis::Y);
	AddMovementInput(direction, Value);
}

void APlayerCharacter::OnLeftTriggerClicked()
{
	leftHandGrab_Server();
}

void APlayerCharacter::OnLeftTriggerReleased()
{
	leftHandRelease_Server();
}

void APlayerCharacter::OnRightTriggerClicked()
{
	rightHandGrab_Server();
}

void APlayerCharacter::OnRightTriggerReleased()
{
	rightHandRelease_Server();
}

void APlayerCharacter::OnTeleportPressedLeft()
{
	teleportComp->Enable(LeftMotionController);
}

void APlayerCharacter::OnTeleportPressedRight()
{
	teleportComp->Enable(RightMotionController);
}

void APlayerCharacter::OnSideGripButtonLeft()
{
	leftHandDrop_Server();
}

void APlayerCharacter::OnSideGripButtonRight()
{
	rightHandDrop_Server();
}

void APlayerCharacter::OnTeleportReleased()
{
	auto result = teleportComp->GetTeleportResult();
	teleportComp->Disable();

	if (result.ShouldTeleport)
	{
		if (Role >= ROLE_Authority)
		{
			performTeleport_NetMulticast(result.Position, GetActorRotation());
		}
		else
		{
			requestTeleport_Server(result.Position, GetActorRotation());
		}		
	}
}

// Left hand.
bool APlayerCharacter::leftHandGrab_Server_Validate()
{
	return true;
}

void APlayerCharacter::leftHandGrab_Server_Implementation()
{
	CastChecked<UVRControllerComponent>(LeftMotionController)->UseGrippedActors();
	CastChecked<UVRControllerComponent>(LeftMotionController)->GrabNearestActor(*leftGrabSphere);
}

bool APlayerCharacter::leftHandRelease_Server_Validate()
{
	return true;
}

void APlayerCharacter::leftHandRelease_Server_Implementation()
{
	CastChecked<UVRControllerComponent>(LeftMotionController)->EndUseGrippedActors();
	CastChecked<UVRControllerComponent>(LeftMotionController)->DropManipulationGrips();
}

bool APlayerCharacter::leftHandDrop_Server_Validate()
{
	return true;
}

void APlayerCharacter::leftHandDrop_Server_Implementation()
{
	CastChecked<UVRControllerComponent>(RightMotionController)->EndUseGrippedActors();
	CastChecked<UVRControllerComponent>(LeftMotionController)->DropAllGrips();
}

// Right hand.
bool APlayerCharacter::rightHandGrab_Server_Validate()
{
	return true;
}

void APlayerCharacter::rightHandGrab_Server_Implementation()
{
	CastChecked<UVRControllerComponent>(RightMotionController)->UseGrippedActors();
	CastChecked<UVRControllerComponent>(RightMotionController)->GrabNearestActor(*rightGrabSphere);
}

bool APlayerCharacter::rightHandRelease_Server_Validate()
{
	return true;
}

void APlayerCharacter::rightHandRelease_Server_Implementation()
{
	CastChecked<UVRControllerComponent>(RightMotionController)->EndUseGrippedActors();
	CastChecked<UVRControllerComponent>(RightMotionController)->DropManipulationGrips();
}

bool APlayerCharacter::rightHandDrop_Server_Validate()
{
	return true;
}

void APlayerCharacter::rightHandDrop_Server_Implementation()
{
	CastChecked<UVRControllerComponent>(RightMotionController)->EndUseGrippedActors();
	CastChecked<UVRControllerComponent>(RightMotionController)->DropAllGrips();
}

bool APlayerCharacter::requestTeleport_Server_Validate(FVector Location, FRotator Rotation)
{
	//you could check if teleport is realistic/possible to prevent cheats
	return true;
}

void APlayerCharacter::requestTeleport_Server_Implementation(FVector Location, FRotator Rotation)
{
	performTeleport_NetMulticast(Location, Rotation);
}

void APlayerCharacter::performTeleport_NetMulticast_Implementation(FVector Location, FRotator Rotation)
{
	TeleportTo(Location, Rotation, false, true);
}