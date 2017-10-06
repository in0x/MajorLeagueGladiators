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

	/** Starts the online game using the session name in the PlayerState */
	UFUNCTION(reliable, client)
	void ClientStartOnlineGame();

	/** Ends the online game using the session name in the PlayerState */
	UFUNCTION(reliable, client)
	void ClientEndOnlineGame();

	FTimerHandle TimerHandle_ClientStartOnlineGame;
};
