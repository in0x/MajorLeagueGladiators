// Fill out your copyright notice in the Description page of Project Settings.

#include "Prototype.h"
#include "PlayerCharacter.h"
#include "IPlayerCharacterMotionController.h"
#include <algorithm>

namespace 
{
	AActor* GetOverlapping(UPrimitiveComponent& component)
	{
		TArray<AActor*> overlaps;
		component.GetOverlappingActors(overlaps);

		overlaps = overlaps.FilterByPredicate([](AActor* pActor)
		{
			return Cast<IVRGripInterface>(pActor->GetRootComponent()) != nullptr;
		});

		if (overlaps.Num() == 0)
		{
			return nullptr;
		}
		else if (overlaps.Num() == 1)
			return *overlaps.GetData();

		AActor* closest = std::min(*overlaps.GetData(), overlaps.Last(), [&component](auto a, auto b)
		{
			return FVector::DistSquared((a)->GetRootComponent()->GetComponentLocation(), component.GetComponentLocation())
				 < FVector::DistSquared((b)->GetRootComponent()->GetComponentLocation(), component.GetComponentLocation());
		});

		return closest;
	}

	bool Grab(UGripMotionControllerComponent* hand)
	{
		AActor* closest = nullptr;
		auto components = hand->GetAttachChildren();
		components.Add(hand);

		for (auto comp : components)
		{
			UPrimitiveComponent* primitveComp = Cast<UPrimitiveComponent>(comp);

			if (primitveComp)
			{
				closest = GetOverlapping(*primitveComp);

				if (closest)
				{
					hand->GripActor(closest, closest->GetTransform());
					return true;
				}
			}
		}
	
		return false;
	}
}

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

	if (!GEngine->HMDDevice.IsValid() || !GEngine->HMDDevice->IsHMDConnected()) 
	{
		pHandMotionController = std::make_unique<HandMotionController>(this);
	}	

	FAttachmentTransformRules meshRules(EAttachmentRule::SnapToTarget, false);
	
	leftMesh->AttachToComponent(LeftMotionController, meshRules);
	rightMesh->AttachToComponent(RightMotionController, meshRules);
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

	playerInputComponent->BindAxis("LeftTrigger", this, &APlayerCharacter::OnLeftTrigger);
	playerInputComponent->BindAxis("RightTrigger", this, &APlayerCharacter::OnRightTrigger);
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

void APlayerCharacter::OnLeftTrigger(float value)
{
	if (value > 0.f)
	{
		GEngine->AddOnScreenDebugMessage(-1, 0.5f, FColor::Green, FString::Printf(TEXT("LeftTrigger: %f"), value));
		::Grab(LeftMotionController);
	}
}

void APlayerCharacter::OnRightTrigger(float value)
{
	if (value > 0.f)
	{
		GEngine->AddOnScreenDebugMessage(-1, 0.5f, FColor::Green, FString::Printf(TEXT("RightTrigger: %f"), value));
		::Grab(RightMotionController);
	}
}


