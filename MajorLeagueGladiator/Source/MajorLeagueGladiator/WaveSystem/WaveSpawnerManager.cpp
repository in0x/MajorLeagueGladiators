#include "MajorLeagueGladiator.h"
#include "WaveSpawnerManager.h"
#include "WaveSystem/WaveSpawner.h"

AWaveSpawnerManager::AWaveSpawnerManager()
{
}

void AWaveSpawnerManager::BeginPlay()
{
	Super::BeginPlay();
}

void AWaveSpawnerManager::GatherSpawners()
{
	TArray<AActor*> FoundActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AWaveSpawner::StaticClass(), FoundActors);
	
	for (AActor* actor : FoundActors)
	{
		AWaveSpawner* waveSpawner = CastChecked<AWaveSpawner>(actor);

		const int SpawnGroupIndex = waveSpawner->GetSpawnGroupIndex();
		FWaveSpawnerGroup& spawnerGroup = spawnGroups.FindOrAdd(SpawnGroupIndex);
		spawnerGroup.WaveSpawners.Add(waveSpawner);		
	}
}
