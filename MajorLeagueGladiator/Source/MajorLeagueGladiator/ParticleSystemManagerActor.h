// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "EmitterSpawnParams.h"
#include "SoundParams.h"
#include "ParticleSystemManagerActor.generated.h"

// TODO: Rename, because it manages sound and particle effects

UCLASS()
class MAJORLEAGUEGLADIATOR_API AParticleSystemManagerActor : public AActor
{
	GENERATED_BODY()
	
public:	
	AParticleSystemManagerActor();

	void CreateParticleSystemAtLocation(const FEmitterSpawnParams& Params) const;

	void PlaySoundAtLocation(const FSoundParams& Params) const;

private:
	UFUNCTION(Server, unreliable, WithValidation)
	void createParticleSystem_Server(const FEmitterSpawnParams& Params) const;
	
	UFUNCTION(NetMulticast, unreliable)
	void createParticleSystem_NetMulticast(const FEmitterSpawnParams& Params) const;

	UFUNCTION(Server, unreliable, WithValidation)
	void playSoundAtLocation_Server(const FSoundParams& Params) const;

	UFUNCTION(NetMulticast, unreliable)
	void playSoundAtLocation_NetMulticast(const FSoundParams& Params) const;
};
