// Fill out your copyright notice in the Description page of Project Settings.

#include "MajorLeagueGladiator.h"
#include "MlgPlayerController.h"

AMlgPlayerController::AMlgPlayerController()
{
	static ConstructorHelpers::FObjectFinder<UForceFeedbackEffect> myRumble(TEXT("/Game/RumbleFiles/ControllerRumble"));
	
	rumbleTest = myRumble.Object;

}

