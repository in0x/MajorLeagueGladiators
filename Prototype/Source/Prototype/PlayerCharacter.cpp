// Fill out your copyright notice in the Description page of Project Settings.

#include "Prototype.h"
#include "PlayerCharacter.h"

APlayerCharacter::APlayerCharacter(const FObjectInitializer& ObjectInitializer /*= FObjectInitializer::Get()*/)
	: Super(ObjectInitializer)
{
	bUseControllerRotationPitch = true;
}

void APlayerCharacter::SetupPlayerInputComponent(UInputComponent* playerInputComponent)
{
	Super::SetupPlayerInputComponent(playerInputComponent);

	//playerInputComponent->BindAxis("Turn", this, &APlayerCharacter::AddControllerYawInput);
	//playerInputComponent->BindAxis("LookUp", this, &APlayerCharacter::AddControllerPitchInput);
}


