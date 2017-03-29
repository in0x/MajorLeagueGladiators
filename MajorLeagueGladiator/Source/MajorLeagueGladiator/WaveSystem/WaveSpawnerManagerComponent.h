// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "SpawnCommand.h"
#include "WaveSpawnerGroup.h"

#include "WaveSpawnerManagerComponent.generated.h"

struct FEnemyDefinition;
struct FWaveDefiniton;
struct FWaveLayoutDefiniton;

UCLASS()
class MAJORLEAGUEGLADIATOR_API UWaveSpawnerManagerComponent : public UActorComponent
{
	GENERATED_BODY()
public:
	UWaveSpawnerManagerComponent();

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
