// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/GameState.h"
#include "MlgGameState.generated.h"

class UReplicatedEffectsComponent;
class UMlgAchievementsComponent;
class UWaveSystemComponent;

UCLASS()
class MAJORLEAGUEGLADIATOR_API AMlgGameState : public AGameState
{
	GENERATED_BODY()
		
public:
	AMlgGameState(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
	
private:
	UPROPERTY()
	UReplicatedEffectsComponent* replicatedEffectsComponent;

	UPROPERTY()
	UMlgAchievementsComponent* achievementsComponent;

	UPROPERTY(EditAnywhere)
	UWaveSystemComponent* waveSystemComponent;
};
