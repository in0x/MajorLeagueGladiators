// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"

#include "SpawnCommand.h"

#include "WaveSpawner.generated.h"

struct FEnemyDefinition;

UCLASS()
class MAJORLEAGUEGLADIATOR_API AWaveSpawner : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AWaveSpawner();

	void Init(const UDataTable* EnemyDefinitions);

	void AddToNextWavePool(const FSpawnCommand& spawnCommand);
	void SpawnWave(float WaitTimeBeforeSpawning, float SpawningDuration);
	bool IsSpawningFinished() const;
	int GetSpawnGroupIndex() const { return spawnGroup; }

private:

	void SpawnEnemy();
	UClass* GetAndRemoveNextEnemyClass();

	void FinishWave();
	
	UPROPERTY(Transient)
	TArray<FSpawnCommand> remainingSpawnPool;

	UPROPERTY(Transient)
	const UDataTable* enemyDefinitions;

	UPROPERTY(EditAnywhere)
	int spawnGroup;

	FTimerHandle spawnTimerHandle;

	bool isCurrentlySpawning;
};
