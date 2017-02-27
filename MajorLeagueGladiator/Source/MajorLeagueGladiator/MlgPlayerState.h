// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/PlayerState.h"
#include "MlgPlayerState.generated.h"

UCLASS()
class MAJORLEAGUEGLADIATOR_API AMlgPlayerState : public APlayerState
{
	GENERATED_BODY()
	
public:
	bool IsSameTeam(const AMlgPlayerState* otherPlayerState) const;
	
};
