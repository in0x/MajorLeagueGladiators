// Fill out your copyright notice in the Description page of Project Settings.

#include "MajorLeagueGladiator.h"
#include "WaveSpawner.h"

AWaveSpawner::AWaveSpawner()
{
	bNetLoadOnClient = false;
}

void AWaveSpawner::Init(const UDataTable* EnemyDefinitions)
{
	enemyDefinitions = EnemyDefinitions;
}

void AWaveSpawner::SpawnWave(const TArray<FSpawnCommand>& SpawnPool, float WaitTimeBeforeSpawning, float TotalSpawnTime)
{
	if (!IsSpawningFinished())
	{
		UE_LOG(LogTemp, Warning, TEXT("Wave was starte while previous wave is still running"));
		FinishWave();
	}

	
	int numberOfSpawnedEnemies = 0;
	for (const FSpawnCommand& spawnCommand : SpawnPool)
	{
		numberOfSpawnedEnemies += spawnCommand.enemyCount;
	}

	const float intervalTime = TotalSpawnTime / numberOfSpawnedEnemies;


	remainingSpawnPool = SpawnPool;
	FTimerManager& timermanager = GetWorldTimerManager();
	timermanager.SetTimer(spawnTimerHandle, this, &AWaveSpawner::SpawnEnemy, intervalTime, true, WaitTimeBeforeSpawning);
}

bool AWaveSpawner::IsSpawningFinished() const
{
	return remainingSpawnPool.Num() <= 0;
}

void AWaveSpawner::SpawnEnemy()
{
	check(HasAuthority());
	UClass* enemyClass = GetAndRemoveNextEnemyClass();
	UWorld* world = GetWorld();

	AActor* spawnedActor = world->SpawnActor<AActor>(enemyClass, GetActorTransform());
}

UClass* AWaveSpawner::GetAndRemoveNextEnemyClass()
{
	if (IsSpawningFinished())
	{
		UE_LOG(LogTemp, Warning, TEXT("Trying getting next enemy while spawning has already been finished"));
		return nullptr;
	}

	UClass* enemyClass = remainingSpawnPool[0].enemyClass;
	remainingSpawnPool[0].enemyCount -= 1;
	if (remainingSpawnPool[0].enemyCount <= 0)
	{
		remainingSpawnPool.RemoveAt(0);
		if (IsSpawningFinished())
		{
			FinishWave();
		}
	}

	return enemyClass;
}

void AWaveSpawner::FinishWave()
{
	FTimerManager& timermanager = GetWorldTimerManager();
	timermanager.ClearTimer(spawnTimerHandle);
}