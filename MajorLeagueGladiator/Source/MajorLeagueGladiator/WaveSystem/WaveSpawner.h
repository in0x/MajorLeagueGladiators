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
	void SpawnWave(const TArray<FSpawnCommand>& SpawnPool, float WaitTimeBeforeSpawning, float TotalSpawnTime);
	bool IsSpawningFinished() const;

private:

	void SpawnEnemy();
	const FEnemyDefinition* GetAndRemoveNextEnemyDefiniton();

	const FEnemyDefinition* FindEnemyDefiniton(FName Name);

	void FinishWave();
	
	UPROPERTY(Transient)
	TArray<FSpawnCommand> remainingSpawnPool;

	UPROPERTY(Transient)
	const UDataTable* enemyDefinitions;

	FTimerHandle spawnTimerHandle;
	float waitTimeBeforeSpawning;
	float spawnInterval;
};
