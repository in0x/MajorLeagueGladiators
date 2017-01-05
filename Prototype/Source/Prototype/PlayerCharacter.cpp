// Fill out your copyright notice in the Description page of Project Settings.

#include "Prototype.h"
#include "PlayerCharacter.h"
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

	/*leftGrabSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	rightGrabSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);*/

	// ArrowComponent is used to determine position and direction for teleport.
	//vrArrow = CastChecked<UArrowComponent>(GetComponentByClass(UArrowComponent::StaticClass())); 

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

	playerInputComponent->BindAction("TeleportPressLeft", EInputEvent::IE_Pressed, this, &APlayerCharacter::OnTeleportPressedLeft);
	playerInputComponent->BindAction("TeleportPressRight", EInputEvent::IE_Pressed, this, &APlayerCharacter::OnTeleportPressedRight);

	playerInputComponent->BindAction("TeleportPressLeft", EInputEvent::IE_Released, this, &APlayerCharacter::OnTeleportReleased);
	playerInputComponent->BindAction("TeleportPressRight", EInputEvent::IE_Released, this, &APlayerCharacter::OnTeleportReleased);

	playerInputComponent->BindAction("SideGripButtonLeft", EInputEvent::IE_Pressed, this, &APlayerCharacter::OnSideGripButtonLeft);
	playerInputComponent->BindAction("SideGripButtonRight", EInputEvent::IE_Pressed, this, &APlayerCharacter::OnSideGripButtonRight);
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
}

void APlayerCharacter::OnRightTriggerAxis(float value)
{
}

void APlayerCharacter::OnLeftTriggerClicked()
{
	CastChecked<UVRControllerComponent>(LeftMotionController)->UseGrippedActors();
	CastChecked<UVRControllerComponent>(LeftMotionController)->GrabNearestActor(*leftGrabSphere);
}

void APlayerCharacter::OnLeftTriggerReleased()
{
	CastChecked<UVRControllerComponent>(LeftMotionController)->EndUseGrippedActors();
	CastChecked<UVRControllerComponent>(LeftMotionController)->DropManipulationGrips();
}

void APlayerCharacter::OnRightTriggerClicked()
{
	CastChecked<UVRControllerComponent>(RightMotionController)->UseGrippedActors();
	CastChecked<UVRControllerComponent>(RightMotionController)->GrabNearestActor(*rightGrabSphere);
}

void APlayerCharacter::OnRightTriggerReleased()
{
	CastChecked<UVRControllerComponent>(RightMotionController)->EndUseGrippedActors();
	CastChecked<UVRControllerComponent>(RightMotionController)->DropManipulationGrips();
}

void APlayerCharacter::OnTeleportPressedLeft()
{
	teleportComp->Enable(LeftMotionController);
}

void APlayerCharacter::OnTeleportPressedRight()
{
	teleportComp->Enable(RightMotionController);
}

void APlayerCharacter::OnTeleportReleased()
{
	auto result = teleportComp->GetTeleportData();
	teleportComp->Disable();

	if (result.ShouldTeleport)
	{
		auto success = TeleportTo(result.Position, GetActorRotation(), false, true);
	}
}

void APlayerCharacter::OnSideGripButtonLeft()
{
	CastChecked<UVRControllerComponent>(LeftMotionController)->DropAllGrips();
}

void APlayerCharacter::OnSideGripButtonRight()
{
	CastChecked<UVRControllerComponent>(RightMotionController)->DropAllGrips();
}