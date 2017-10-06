// Fill out your copyright notice in the Description page of Project Settings.

#include "MajorLeagueGladiator.h"
#include "MlgPlayerController.h"
#include "MlgPlayerState.h"
#include "OnlineSubsystem.h"
#include "OnlineSessionInterface.h"

AMlgPlayerController::AMlgPlayerController()
{
}

/** Starts the online game using the session name in the MlgPlayerState */
void AMlgPlayerController::ClientStartOnlineGame_Implementation()
{
	if (!IsPrimaryPlayer())
		return;

	AMlgPlayerState* MlgPlayerState = Cast<AMlgPlayerState>(PlayerState);
	if (MlgPlayerState)
	{
		IOnlineSubsystem* OnlineSub = IOnlineSubsystem::Get();
		if (OnlineSub)
		{
			IOnlineSessionPtr Sessions = OnlineSub->GetSessionInterface();
			if (Sessions.IsValid())
			{
				UE_LOG(LogOnline, Log, TEXT("Starting session %s on client"), *MlgPlayerState->SessionName.ToString());
				Sessions->StartSession(MlgPlayerState->SessionName);
			}
		}
	}
	else
	{
		// Keep retrying until player state is replicated
		GetWorld()->GetTimerManager().SetTimer(TimerHandle_ClientStartOnlineGame, this, &AMlgPlayerController::ClientStartOnlineGame_Implementation, 0.2f, false);
	}
}

/** Ends the online game using the session name in the MlgPlayerState */
void AMlgPlayerController::ClientEndOnlineGame_Implementation()
{
	if (!IsPrimaryPlayer())
		return;

	AMlgPlayerState* MlgPlayerState = Cast<AMlgPlayerState>(PlayerState);
	if (MlgPlayerState)
	{
		IOnlineSubsystem* OnlineSub = IOnlineSubsystem::Get();
		if (OnlineSub)
		{
			IOnlineSessionPtr Sessions = OnlineSub->GetSessionInterface();
			if (Sessions.IsValid())
			{
				UE_LOG(LogOnline, Log, TEXT("Ending session %s on client"), *MlgPlayerState->SessionName.ToString());
				Sessions->EndSession(MlgPlayerState->SessionName);
			}
		}
	}
}
