// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"

#include "SpawnCommand.h"
#include "WaveSpawnerGroup.h"

#include "WaveSpawnerManager.generated.h"

struct FEnemyDefinition;

UCLASS()
class MAJORLEAGUEGLADIATOR_API AWaveSpawnerManager : public AActor
{
	GENERATED_BODY()
public:
	AWaveSpawnerManager();
	void BeginPlay();
	void GatherSpawners();
private:
	UPROPERTY(EditAnywhere)
	TArray<UDataTable*> layoutDefinitions;

	UPROPERTY(EditAnywhere)
	UDataTable* waveDefiniton;

	UPROPERTY(Transient)
	TMap<int, FWaveSpawnerGroup> spawnGroups;
};
