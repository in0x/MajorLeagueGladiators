// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/GameStateBase.h"
#include "MlgGameState.generated.h"

class AEffectsManagerActor;

UCLASS()
class MAJORLEAGUEGLADIATOR_API AMlgGameState : public AGameStateBase
{
	GENERATED_BODY()
		
public:
	AMlgGameState(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	AEffectsManagerActor* GetParticleSystemManager();

	void SetGetParticleSystemManager(AEffectsManagerActor* psManager);
	
private:
	UPROPERTY(Replicated, Transient)
	AEffectsManagerActor* particleSystemManager;
};
