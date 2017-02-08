// Fill out your copyright notice in the Description page of Project Settings.

#include "MajorLeagueGladiator.h"
#include "MlgPlayerController.h"

AMlgPlayerController::AMlgPlayerController()
{
	const static ConstructorHelpers::FObjectFinder<UForceFeedbackEffect> helperShortLeft(TEXT("/Game/RumbleFiles/RumbleShortLeft"));
	const static ConstructorHelpers::FObjectFinder<UForceFeedbackEffect> helperShortRight(TEXT("/Game/RumbleFiles/RumbleShortRight"));

	rumbleShortLeft = NewObject<UForceFeedbackEffect>(helperShortLeft.Object);
	rumbleShortRight = NewObject<UForceFeedbackEffect>(helperShortRight.Object);
}

