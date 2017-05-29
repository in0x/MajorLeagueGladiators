// Fill out your copyright notice in the Description page of Project Settings.

#include "MajorLeagueGladiator.h"
#include "MenuCharacter.h"
#include "Menu/MenuGameMode.h"
namespace
{
	const FName NO_COLLISION_PROFILE_NAME("NoCollision");
}

AMenuCharacter::AMenuCharacter(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	BaseEyeHeight = 0.0f;
	CrouchedEyeHeight = 0.0f;

	leftMesh = ObjectInitializer.CreateDefaultSubobject<UStaticMeshComponent>(this, TEXT("LeftMesh"));
	rightMesh = ObjectInitializer.CreateDefaultSubobject<UStaticMeshComponent>(this, TEXT("RightMesh"));

	leftMesh->SetupAttachment(LeftMotionController);
	rightMesh->SetupAttachment(RightMotionController);

	ConstructorHelpers::FObjectFinder<UStaticMesh> viveMesh(TEXT("StaticMesh'/Game/MVRCFPS_Assets/vive_controller.vive_controller'"));
	if (viveMesh.Succeeded())
	{
		leftMesh->SetStaticMesh(viveMesh.Object);
		rightMesh->SetStaticMesh(viveMesh.Object);
	}

	LeftMotionController->SetCollisionProfileName(NO_COLLISION_PROFILE_NAME);
	RightMotionController->SetCollisionProfileName(NO_COLLISION_PROFILE_NAME);
	leftMesh->SetCollisionProfileName(NO_COLLISION_PROFILE_NAME);
	rightMesh->SetCollisionProfileName(NO_COLLISION_PROFILE_NAME);
}

void AMenuCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	PlayerInputComponent->BindAction("LeftTriggerClicked", EInputEvent::IE_Pressed, this, &AMenuCharacter::OnLeftTriggerClicked);
	PlayerInputComponent->BindAction("RightTriggerClicked", EInputEvent::IE_Pressed, this, &AMenuCharacter::OnRightTriggerClicked);
}


void AMenuCharacter::OnLeftTriggerClicked()
{
	GetWorld()->GetAuthGameMode<AMenuGameMode>()->hostGame();
}

void AMenuCharacter::OnRightTriggerClicked()
{
	GetWorld()->GetAuthGameMode<AMenuGameMode>()->findGame();
}

