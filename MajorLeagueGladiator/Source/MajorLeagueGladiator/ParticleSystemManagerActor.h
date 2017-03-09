// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "ParticleSystemManagerActor.generated.h"

UCLASS()
class MAJORLEAGUEGLADIATOR_API AParticleSystemManagerActor : public AActor
{
	GENERATED_BODY()
	
public:	
	AParticleSystemManagerActor();

	void SpawnParticleSystemAtLocation(UParticleSystem* ParticleSystemTemplate, FTransform Trans, bool AutoDestroy = true);

private:

	UFUNCTION(NetMulticast, unreliable)
	void CreateParticleSystem_NetMulticast(UParticleSystem* EmitterTemplate, UWorld* World, AActor* Actor, bool bAutoDestroy);

	UPROPERTY()
	TArray<UParticleSystemComponent*> particleSystems;

};
