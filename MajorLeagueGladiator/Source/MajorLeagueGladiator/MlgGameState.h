// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/GameStateBase.h"
#include "MlgGameState.generated.h"

class AParticleSystemManagerActor;

/**
 * 
 */
UCLASS()
class MAJORLEAGUEGLADIATOR_API AMlgGameState : public AGameStateBase
{
	GENERATED_BODY()
	
	
public:

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	AParticleSystemManagerActor* GetParticleSystemManager();

	void SetGetParticleSystemManager(AParticleSystemManagerActor* psManager);

private:

	UPROPERTY(Replicated)
	AParticleSystemManagerActor* particleSystemManager;
	
};
