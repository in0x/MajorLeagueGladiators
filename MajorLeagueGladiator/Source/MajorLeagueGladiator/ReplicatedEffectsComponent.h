// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Components/ActorComponent.h"
#include "EmitterSpawnParams.h"
#include "BeamEmitterSpawnParams.h"
#include "SoundParams.h"
#include "ReplicatedEffectsComponent.generated.h"

UCLASS()
class MAJORLEAGUEGLADIATOR_API UReplicatedEffectsComponent : public UActorComponent
{
	GENERATED_BODY()
	
public:	
	UReplicatedEffectsComponent();

	void CreateParticleSystemAtLocation(const FEmitterSpawnParams& Params) const;

	void CreateBeamParticleSystemAtLocation(const FBeamEmitterSpawnParams& Params) const;

	void PlaySoundNetworked(const FSoundParams& Params) const;

	UFUNCTION(Server, reliable, WithValidation)
	void ApplyPointDamage_Server(AActor* DamagedActor, float BaseDamage, const FVector& HitFromDirection, const FHitResult& HitInfo, AController* EventInstigator, AActor* DamageCauser, TSubclassOf<class UDamageType> DamageTypeClass);

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
