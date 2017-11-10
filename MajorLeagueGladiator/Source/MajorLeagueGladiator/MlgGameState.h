// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/GameState.h"
#include "MlgGameState.generated.h"

class UReplicatedEffectsComponent;
class UWaveSystemComponent;

UCLASS(Config=Game)
class MAJORLEAGUEGLADIATOR_API AMlgGameState : public AGameState
{
	GENERATED_BODY()
		
public:
	AMlgGameState(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
	virtual void BeginPlay() override;

private:
	UFUNCTION()
	void WaveCleared(int32 WaveNumber);

	UPROPERTY()
	UReplicatedEffectsComponent* replicatedEffectsComponent;

	UPROPERTY(EditAnywhere)
	UWaveSystemComponent* waveSystemComponent;

	UPROPERTY(Config)
	int32 easyAchievementWave;

	UPROPERTY(Config)
	int32 mediumAchievementWave;

	UPROPERTY(Config)
	int32 hardAchievementWave;

	UPROPERTY(Config)
	FString beatEasyAchievementId;

	UPROPERTY(Config)
	FString beatMediumAchievementId;

	UPROPERTY(Config)
	FString beatHardAchievementId;
};
