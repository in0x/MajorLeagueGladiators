// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Components/ActorComponent.h"
#include "EmitterSpawnParams.h"

#include "PredictedEffectsComponent.generated.h"

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class MAJORLEAGUEGLADIATOR_API UPredictedEffectsComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UPredictedEffectsComponent();
	
	bool IsLocallyControlled() const;

	void CreateParticleSystemNetworkedPredicted(const FEmitterSpawnParams& Params) const;
	void CreateParticleSystemLocal(const FEmitterSpawnParams& Params) const;

private:

	UFUNCTION(Server, reliable, WithValidation)
	void createParticleSystem_Server(const FEmitterSpawnParams& Params) const;

	UFUNCTION(NetMulticast, reliable)
	void createParticleSystem_NetMulticast(const FEmitterSpawnParams& Params) const;
};
