// Fill out your copyright notice in the Description page of Project Settings.

#include "MajorLeagueGladiator.h"
#include "MlgGameState.h"
#include "EffectsManagerActor.h"
#include "WaveSystem/WaveSystemComponent.h"

AMlgGameState::AMlgGameState(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	ObjectInitializer.CreateDefaultSubobject<UWaveSystemComponent>(this, TEXT("WaveSystemComponent"));
}

void AMlgGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AMlgGameState, effectsManager);
}

AEffectsManagerActor* AMlgGameState::GetEffectsManager()
{
	return effectsManager;
}

void AMlgGameState::SetEffectsManager(AEffectsManagerActor* fxManager)
{
	effectsManager = fxManager;
}
