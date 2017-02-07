// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "VRExpansion/VRPlayerController.h"
#include "MlgPlayerController.generated.h"

UCLASS()
class MAJORLEAGUEGLADIATOR_API AMlgPlayerController : public AVRPlayerController
{
	GENERATED_BODY()
public:
	AMlgPlayerController();
	UForceFeedbackEffect* rumbleTest;
};
