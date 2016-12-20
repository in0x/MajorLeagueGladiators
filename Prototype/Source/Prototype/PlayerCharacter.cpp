// Fill out your copyright notice in the Description page of Project Settings.

#include "Prototype.h"
#include "PlayerCharacter.h"
#include "HandComponent.h"

APlayerCharacter::APlayerCharacter(const FObjectInitializer& ObjectInitializer /*= FObjectInitializer::Get()*/)
	: Super(ObjectInitializer)
{
	bUseControllerRotationPitch = true;

	leftMesh = ObjectInitializer.CreateDefaultSubobject<UStaticMeshComponent>(this, TEXT("LeftMesh"));
	rightMesh = ObjectInitializer.CreateDefaultSubobject<UStaticMeshComponent>(this, TEXT("RightMesh"));
}

void APlayerCharacter::BeginPlay()
{
	Super::BeginPlay();
	if (GEngine->HMDDevice.IsValid() && GEngine->HMDDevice->IsHMDConnected()) // Check if we are in VR mode
	{
		leftHand = LeftMotionController;
		rightHand = RightMotionController;

		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("HMD is valid"));
	}
	else // If not, assign placeholder hand objects
	{
		auto lh = NewObject<UHandComponent>();
		lh->SetControllerHand(EControllerHand::Left);
		leftHand = lh;
		 
		auto rh = NewObject<UHandComponent>();
		rh->SetControllerHand(EControllerHand::Right);
		rightHand = rh;

		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow, TEXT("HMD is not valid"));

		FAttachmentTransformRules handRules(EAttachmentRule::KeepRelative, false);

		leftHand->AttachToComponent(RootComponent, handRules);
		rightHand->AttachToComponent(RootComponent, handRules);
	}	

	FAttachmentTransformRules meshRules(EAttachmentRule::SnapToTarget, false);
	
	leftMesh->AttachToComponent(leftHand, meshRules);
	rightMesh->AttachToComponent(rightHand, meshRules);
	
	leftHand->RegisterComponent();
	rightHand->RegisterComponent();
}

void APlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	TArray<IMotionController*> MotionControllers = IModularFeatures::Get().GetModularFeatureImplementations<IMotionController>(IMotionController::GetModularFeatureName());
	for (auto MotionController : MotionControllers)
	{
		FRotator rot;
		FVector vec;

		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, FString::FromInt(MotionControllers.Num()));

		if ((MotionController != nullptr) && MotionController->GetControllerOrientationAndPosition(0, EControllerHand::Left, rot, vec))
		{
			auto status = (EBPTrackingStatus)MotionController->GetControllerTrackingStatus(0, EControllerHand::Right);
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, TEXT("MotionController found"));
		}
		else
		{
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("MotionController not found"));
		}
	}
}

void APlayerCharacter::SetupPlayerInputComponent(UInputComponent* playerInputComponent)
{
	Super::SetupPlayerInputComponent(playerInputComponent);

	playerInputComponent->BindAxis("MoveForward", this, &APlayerCharacter::MoveForward);
	playerInputComponent->BindAxis("MoveRight", this, &APlayerCharacter::MoveRight);

	playerInputComponent->BindAxis("Turn", this, &APlayerCharacter::AddControllerYawInput);
	playerInputComponent->BindAxis("LookUp", this, &APlayerCharacter::AddControllerPitchInput);
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

