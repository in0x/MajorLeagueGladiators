// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "ParticleSystemManagerActor.generated.h"

UENUM()
enum class EParticleSystem : uint8
{
	HitscanBeam 	UMETA(DisplayName = "HitscanBeam")
};

UCLASS()
class MAJORLEAGUEGLADIATOR_API AParticleSystemManagerActor : public AActor
{
	GENERATED_BODY()
	
public:	
	AParticleSystemManagerActor();

	void SpawnParticleSystemAtLocation(EParticleSystem particleSystem, FTransform Trans, bool AutoDestroy = true);

private:

	UFUNCTION(NetMulticast, unreliable)
	void CreateParticleSystem_NetMulticast(UParticleSystem* EmitterTemplate, UWorld* World, AActor* Actor, FTransform Trans, bool bAutoDestroy);

	UPROPERTY(EditAnywhere)
	TArray<UParticleSystem*> particleSystems;

};
