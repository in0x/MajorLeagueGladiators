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

	void CreateParticleSystemMain(UParticleSystem* EmitterTemplate, FTransform Trans, bool bAutoDestroy);

private:

	UFUNCTION(Server, unreliable, WithValidation)
	void CreateParticleSystem_Server(int Index, FTransform Trans, bool bAutoDestroy);
	
	UFUNCTION(NetMulticast, unreliable)
	void CreateParticleSystem_NetMulticast(int Index, FTransform Trans, bool bAutoDestroy);

	UPROPERTY(EditAnywhere)
	TArray<UParticleSystem*> particleSystems;

};
