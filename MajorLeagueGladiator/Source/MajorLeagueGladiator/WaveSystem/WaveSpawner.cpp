// Fill out your copyright notice in the Description page of Project Settings.

#include "MajorLeagueGladiator.h"
#include "WaveSpawner.h"
#include "WaveSystem/WaveSystemComponent.h"

AWaveSpawner::AWaveSpawner(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
	, isCurrentlySpawning(false)
{
	RootComponent = ObjectInitializer.CreateDefaultSubobject<USceneComponent>(this, "rootComp");
	bNetLoadOnClient = false;
}

void AWaveSpawner::Init(const UDataTable* EnemyDefinitions)
{
	enemyDefinitions = EnemyDefinitions;
}

void AWaveSpawner::AddToNextWavePool(const FSpawnCommand& spawnCommand)
{
	if (!IsSpawningFinished())
	{
		UE_LOG(LogTemp, Warning, TEXT("Wave is was still running when next enmies were added to pool!"));
		FinishWave();
	}
	remainingSpawnPool.Add(spawnCommand);
}

void AWaveSpawner::SpawnWave(float WaitTimeBeforeSpawning, float SpawningDuration)
{	
	int numberOfSpawnedEnemies = 0;
	for (const FSpawnCommand& spawnCommand : remainingSpawnPool)
	{
		numberOfSpawnedEnemies += spawnCommand.enemyCount;
	}

	const float intervalTime = SpawningDuration / numberOfSpawnedEnemies;

	FTimerManager& timermanager = GetWorldTimerManager();
	timermanager.SetTimer(spawnTimerHandle, this, &AWaveSpawner::SpawnEnemy, intervalTime, true, WaitTimeBeforeSpawning);
}

bool AWaveSpawner::IsSpawningFinished() const
{
	return !isCurrentlySpawning;
}

void AWaveSpawner::SpawnEnemy()
{
	check(HasAuthority());
	UClass* enemyClass = GetAndRemoveNextEnemyClass();
	UWorld* world = GetWorld();

	AActor* spawnedActor = world->SpawnActor<AActor>(enemyClass, GetActorTransform());
	spawnedActor->OnDestroyed.AddDynamic(this, &AWaveSpawner::onSpawnedActorDestroyed);
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

void AWaveSpawner::onSpawnedActorDestroyed(AActor* DestroyedActor)
{
	check(HasAuthority());
	GetWorld()->GetGameState()->FindComponentByClass<UWaveSystemComponent>()
		->OnEnemyKilled(CastChecked<ACharacter>(DestroyedActor));
}
