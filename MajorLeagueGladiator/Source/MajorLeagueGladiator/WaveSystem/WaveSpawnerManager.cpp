#include "MajorLeagueGladiator.h"
#include "WaveSpawnerManager.h"
#include "WaveSystem/WaveSpawner.h"
#include "WaveSystem/WaveDefiniton.h"
#include "WaveSystem/WaveLayoutDefinition.h"

namespace
{
	FName IntToFName(int32 Number)
	{
		const FString numberString(FString::FromInt(Number));
		const FName numberName(*numberString);
		return numberName;
	}

	int32 FNameToInt(const FName& Name)
	{
		const FString string = Name.ToString();
		const int32 number = FCString::Atoi(*string);
		return number;
	}
}

AWaveSpawnerManager::AWaveSpawnerManager()
{
}

void AWaveSpawnerManager::BeginPlay()
{
	Super::BeginPlay();
	gatherSpawners();
}

void AWaveSpawnerManager::gatherSpawners()
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


void AWaveSpawnerManager::startWave(int32 WaveNumber)
{
	if (waveEnemiesLeft > 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("Spawning Enemies while there are still enemies left"));
	}

	const FWaveDefiniton* waveDefiniton = getWaveDefinition(WaveNumber);
	const UDataTable* layoutDefinitionTable = waveDefiniton->WaveLayout;

	const TArray<FName> rowNameArray = layoutDefinitionTable->GetRowNames();
	for (const FName& rowName : rowNameArray)
	{
		int32 spawnGroupIndex = FNameToInt(rowName);
		const FWaveLayoutDefiniton* layoutDefiniton = layoutDefinitionTable->FindRow<FWaveLayoutDefiniton>(rowName,
			FString::Printf(TEXT("WaveSpawner Manager, spawnGroup: %d"), spawnGroupIndex));

		spawnForSpawnerGroupIndex(WaveNumber, layoutDefiniton, waveDefiniton);
	}
}

void AWaveSpawnerManager::spawnForSpawnerGroupIndex(int32 SpawnGroupIndex, const FWaveLayoutDefiniton* LayoutDefiniton,
	const FWaveDefiniton* WaveDefinition)
{
	FWaveSpawnerGroup* waveSpawnerGroup = spawnGroups.Find(SpawnGroupIndex);
	if (waveSpawnerGroup == nullptr && waveSpawnerGroup->WaveSpawners.Num() > 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("No Wavespawners for group index %d in level!!"), SpawnGroupIndex);
		return;
	}

	const int32 waveSpawnerCount = waveSpawnerGroup->WaveSpawners.Num();
	TArray<AWaveSpawner*> waveSpawnerArray = waveSpawnerGroup->WaveSpawners;

	// Add Commands to the spawner's pools
	for (const FSpawnDefiniton& spawnDef : LayoutDefiniton->EnemySpawnDefinitons)
	{
		const int32 TotalEnemyAmount = static_cast<int32>(spawnDef.baseAmount * WaveDefinition->enemyNumberModifier);
		const int32 EnemyAmountPerSpawner = TotalEnemyAmount / waveSpawnerCount;
		const int32 EnemyRemainder = TotalEnemyAmount % waveSpawnerCount;

		waveEnemiesLeft += TotalEnemyAmount;

		for (int i = 0; i < waveSpawnerCount; ++i)
		{
			// Decides if an additional enemy needs to be added. Can be 0 or 1
			int32 additionalEnemy = EnemyRemainder / i;
			waveSpawnerArray[i]->AddToNextWavePool({ spawnDef.EnemyClass, EnemyAmountPerSpawner + additionalEnemy });
		}
	}

	// Start the spawners
	for (AWaveSpawner* waveSpawner : waveSpawnerArray)
	{
		waveSpawner->SpawnWave(LayoutDefiniton->WaitTimeBeforeSpawning, LayoutDefiniton->SpawningDuration);
	}
}

const FWaveDefiniton* AWaveSpawnerManager::getWaveDefinition(int32 WaveNumber) const
{
	return waveDefinitonTable->FindRow<FWaveDefiniton>(IntToFName(WaveNumber),
		FString::Printf(TEXT("WaveSpawner Manager, Wave: %d"), WaveNumber));
}

