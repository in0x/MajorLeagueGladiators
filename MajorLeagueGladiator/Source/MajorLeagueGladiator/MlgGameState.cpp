// Fill out your copyright notice in the Description page of Project Settings.

#include "MajorLeagueGladiator.h"
#include "MlgGameState.h"
#include "ParticleSystemManagerActor.h"

void AMlgGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AMlgGameState, particleSystemManager);
}

AParticleSystemManagerActor* AMlgGameState::GetParticleSystemManager()
{
	return particleSystemManager;
}

void AMlgGameState::SetGetParticleSystemManager(AParticleSystemManagerActor* psManager)
{
	particleSystemManager = psManager;
}