// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "SpawnCommand.h"
#include "WaveSpawnerGroup.h"

#include "WaveSpawnerManagerComponent.generated.h"

class AWaveSpawner;
struct FEnemyDefinition;
struct FWaveDefiniton;
struct FWaveLayoutDefiniton;


/*
Accepts request from the WaveSystemComponent to spawn a wave. This class retrieves the right data from
data tables to find out what and how many enemies to spawn. It it discovers all WaveSpawners in the map
and sends spawn request to each of them, according to a to the wave definiton, layout and spawnerGroup
*/
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
	int32 spawnForLayout(const FWaveLayoutDefiniton* LayoutDefiniton, const FWaveDefiniton* WaveDefinition);
	
	TArray<AWaveSpawner*> FindWaveSpawnersByNames(const TArray<FName>& Names);

	UPROPERTY(EditAnywhere)
	UDataTable* waveDefinitonTable;

	UPROPERTY(Transient)
	TMap<FName, AWaveSpawner*> waveSpawners;
};
