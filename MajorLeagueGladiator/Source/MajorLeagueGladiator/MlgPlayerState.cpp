// Fill out your copyright notice in the Description page of Project Settings.

#include "MajorLeagueGladiator.h"
#include "MlgPlayerState.h"


bool AMlgPlayerState::IsSameTeam(const AMlgPlayerState* otherPlayerState) const
{
	return otherPlayerState && team == otherPlayerState->team;
}

