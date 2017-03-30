// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"

#include "SpawnCommand.h"

#include "WaveSpawner.generated.h"

struct FEnemyDefinition;

/*
Gets information from the WaveSpawnerManagerComponent, what, when and how many enemies to spawn, as well
as the total spawning duration. Only calculates the spawning interval, but does not further calculation.
Actually spawns the enemies and registers them to the WaveSystemComponent (currently. last par may change).
*/
UCLASS()
class MAJORLEAGUEGLADIATOR_API AWaveSpawner : public AActor
{
	GENERATED_BODY()
	
public:	
	static const FName INVALID_NAME;

	// Sets default values for this actor's properties
	AWaveSpawner(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	void Init(const UDataTable* EnemyDefinitions);

	void AddToNextWavePool(const FSpawnCommand& spawnCommand);
	void SpawnWave(float WaitTimeBeforeSpawning, float SpawningDuration);
	bool IsSpawningFinished() const;
	const FName& GetUniqueName() const { return uniqueName; }

private:

	void SpawnEnemy();
	UClass* GetAndRemoveNextEnemyClass();

	void FinishWave();

	UFUNCTION()
	void onSpawnedActorDestroyed(AActor* DestroyedActor);
	
	UPROPERTY(Transient)
	TArray<FSpawnCommand> remainingSpawnPool;

	UPROPERTY(Transient)
	const UDataTable* enemyDefinitions;

	UPROPERTY(EditAnywhere)
	FName uniqueName;

	FTimerHandle spawnTimerHandle;

	bool isCurrentlySpawning;
};
