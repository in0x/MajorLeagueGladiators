// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"

#include "SpawnCommand.h"
#include "WaveSpawnerGroup.h"

#include "WaveSpawnerManager.generated.h"

struct FEnemyDefinition;
struct FWaveDefiniton;
struct FWaveLayoutDefiniton;

UCLASS()
class MAJORLEAGUEGLADIATOR_API AWaveSpawnerManager : public AActor
{
	GENERATED_BODY()
public:
	AWaveSpawnerManager();

	int32 StartWave(int32 WaveNumber);
protected:
	void BeginPlay();

private:
	void gatherSpawners();
	
	const FWaveDefiniton* getWaveDefinition(int32 WaveNumber) const;
	int32 spawnForSpawnerGroupIndex(int32 SpawnGroupIndex, const FWaveLayoutDefiniton* LayoutDefiniton,
		const FWaveDefiniton* WaveDefinition);

	UPROPERTY(EditAnywhere)
	UDataTable* waveDefinitonTable;

	UPROPERTY(Transient)
	TMap<int, FWaveSpawnerGroup> spawnGroups;
};
