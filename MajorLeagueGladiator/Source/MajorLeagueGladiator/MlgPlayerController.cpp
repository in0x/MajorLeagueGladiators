// Fill out your copyright notice in the Description page of Project Settings.

#include "MajorLeagueGladiator.h"
#include "MlgPlayerController.h"

AMlgPlayerController::AMlgPlayerController()
{
	const static ConstructorHelpers::FObjectFinder<UForceFeedbackEffect> helperShortLeft(TEXT("/Game/RumbleFiles/RumbleShortLeft"));
	const static ConstructorHelpers::FObjectFinder<UForceFeedbackEffect> helperShortRight(TEXT("/Game/RumbleFiles/RumbleShortRight"));

	rumbleShortLeft = helperShortLeft.Object;
	rumbleShortRight = helperShortRight.Object;
}

UForceFeedbackEffect* AMlgPlayerController::GetRumbleShortLeft()
{
	return rumbleShortLeft;
}

UForceFeedbackEffect* AMlgPlayerController::GetRumbleShortRight()
{
	return rumbleShortRight;
}