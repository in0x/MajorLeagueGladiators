// Fill out your copyright notice in the Description page of Project Settings.

#include "MajorLeagueGladiator.h"
#include "MlgGameState.h"
#include "ReplicatedEffectsComponent.h"
#include "WaveSystem/WaveSystemComponent.h"

AMlgGameState::AMlgGameState(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	waveSystemComponent = ObjectInitializer.CreateDefaultSubobject<UWaveSystemComponent>(this, TEXT("WaveSystemComponent"));
	replicatedEffectsComponent = ObjectInitializer.CreateDefaultSubobject<UReplicatedEffectsComponent>(this, TEXT("ReplicatedEffectsComponent"));
}
