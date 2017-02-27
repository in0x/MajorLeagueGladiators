// Fill out your copyright notice in the Description page of Project Settings.

#include "MajorLeagueGladiator.h"
#include "MlgPlayerState.h"


bool AMlgPlayerState::IsSameTeam(const AMlgPlayerState* otherPlayerState) const
{
	// For now teams there is only the AI Team and the non AI Team. Easiest Solution until we need more teams.
	check(otherPlayerState);
	return bIsABot == otherPlayerState->bIsABot;
}

