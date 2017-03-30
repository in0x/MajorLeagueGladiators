// Fill out your copyright notice in the Description page of Project Settings.

#include "MajorLeagueGladiator.h"
#include "WaveSpawner.h"
#include "WaveSystem/WaveSystemComponent.h"

const FName AWaveSpawner::INVALID_NAME(TEXT("INVALID_NAME"));

AWaveSpawner::AWaveSpawner(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
	, isCurrentlySpawning(false)
	, uniqueName(INVALID_NAME)
{
	RootComponent = ObjectInitializer.CreateDefaultSubobject<USceneComponent>(this, "rootComp");
	bNetLoadOnClient = false;
}

void AWaveSpawner::AddToNextWavePool(const FSpawnCommand& spawnCommand)
{
	if (!IsSpawningFinished())
	{
		UE_LOG(LogTemp, Warning, TEXT("Wave is was still running when next enmies were added to pool!"));
		endSpawning();
	}

	if (spawnCommand.EnemyCount <= 0)
	{
		return;
	}

	remainingSpawnPool.Add(spawnCommand);
}

void AWaveSpawner::BeginSpawning(float WaitTimeBeforeSpawning, float SpawningDuration)
{	
	if (remainingSpawnPool.Num() == 0)
	{
		return;
	}

	int numberOfEnemiesToSpawn = 0;
	for (const FSpawnCommand& spawnCommand : remainingSpawnPool)
	{
		numberOfEnemiesToSpawn += spawnCommand.EnemyCount;
	}

	const float intervalTime = SpawningDuration / numberOfEnemiesToSpawn;
	isCurrentlySpawning = true;

	FTimerManager& timermanager = GetWorldTimerManager();
	timermanager.SetTimer(spawnTimerHandle, this, &AWaveSpawner::spawnEnemy, intervalTime, true, WaitTimeBeforeSpawning);
}

bool AWaveSpawner::IsSpawningFinished() const
{
	return !isCurrentlySpawning;
}

void AWaveSpawner::spawnEnemy()
{
	check(HasAuthority());
	UClass* enemyClass = getAndRemoveNextEnemyClass();
	UWorld* world = GetWorld();

	FActorSpawnParameters spawnParams;
	spawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

	AActor* spawnedActor = world->SpawnActor<AActor>(enemyClass, GetActorTransform(), spawnParams);
	if (spawnedActor == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("Spawning actor failed"));
		return;
	}
	spawnedActor->OnDestroyed.AddDynamic(this, &AWaveSpawner::onSpawnedActorDestroyed);
}

UClass* AWaveSpawner::getAndRemoveNextEnemyClass()
{
	if (IsSpawningFinished())
	{
		UE_LOG(LogTemp, Warning, TEXT("Trying getting next enemy while spawning has already been finished"));
		return nullptr;
	}
	check(remainingSpawnPool.Num() > 0);

	UClass* enemyClass = remainingSpawnPool[0].EnemyClass;
	check(remainingSpawnPool[0].EnemyCount > 0);
	remainingSpawnPool[0].EnemyCount -= 1;
	if (remainingSpawnPool[0].EnemyCount == 0)
	{
		remainingSpawnPool.RemoveAt(0);
		if (remainingSpawnPool.Num() == 0)
		{
			endSpawning();
		}
	}

	return enemyClass;
}

void AWaveSpawner::endSpawning()
{
	FTimerManager& timermanager = GetWorldTimerManager();
	timermanager.ClearTimer(spawnTimerHandle);
	isCurrentlySpawning = false;
}

void AWaveSpawner::onSpawnedActorDestroyed(AActor* DestroyedActor)
{
	// TODO: (FS) possible refactoring
	check(HasAuthority());
	GetWorld()->GetGameState()->FindComponentByClass<UWaveSystemComponent>()
		->OnEnemyKilled(CastChecked<ACharacter>(DestroyedActor));
}
