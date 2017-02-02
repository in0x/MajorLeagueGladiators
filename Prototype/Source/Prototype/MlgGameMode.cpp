// Fill out your copyright notice in the Description page of Project Settings.

#include "Prototype.h"
#include "MlgGameMode.h"
#include "MlgPlayerCharacter.h"
#include "MlgPlayerController.h"

AMlgGameMode::AMlgGameMode(const FObjectInitializer& ObjectInitializer)
{
	DefaultPawnClass = AMlgPlayerCharacter::StaticClass();
	PlayerControllerClass = AMlgPlayerController::StaticClass();
}
