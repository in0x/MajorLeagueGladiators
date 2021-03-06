// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Components/ActorComponent.h"

#include "EmitterSpawnParams.h"
#include "BeamEmitterSpawnParams.h"

#include "SoundParams.h"

#include "PredictedEffectsComponent.generated.h"

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class MAJORLEAGUEGLADIATOR_API UPredictedEffectsComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UPredictedEffectsComponent();
	
	bool IsLocallyControlled() const;

	void CreateParticleSystemNetworkedPredicted(const FEmitterSpawnParams& Params) const;
	void CreateParticleSystemLocal(const FEmitterSpawnParams& Params) const;

	void CreateBeamParticleSystemNetworkedPredicted(const FBeamEmitterSpawnParams& Params) const;
	void CreateBeamParticleSystemLocal(const FBeamEmitterSpawnParams& Params) const;
	
	void PlaySoundNetworkedPredicted(const FSoundParams& Params) const;
	void PlaySoundLocal(const FSoundParams& Params) const;

private:

	UFUNCTION(Server, unreliable, WithValidation)
	void createParticleSystem_Server(const FEmitterSpawnParams& Params) const;

	UFUNCTION(NetMulticast, unreliable)
	void createParticleSystem_NetMulticast(const FEmitterSpawnParams& Params) const;

	UFUNCTION(Server, unreliable, WithValidation)
	void createBeamParticleSystem_Server(const FBeamEmitterSpawnParams& Params) const;

	UFUNCTION(NetMulticast, unreliable)
	void createBeamParticleSystem_NetMulticast(const FBeamEmitterSpawnParams& Params) const;

	UFUNCTION(Server, unreliable, WithValidation)
	void playSound_Server(const FSoundParams& Params) const;

	UFUNCTION(NetMulticast, unreliable)
	void playSound_NetMulticast(const FSoundParams& Params) const;
};
