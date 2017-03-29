#include "MajorLeagueGladiator.h"
#include "WaveSpawnerManagerComponent.h"
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

UWaveSpawnerManagerComponent::UWaveSpawnerManagerComponent()
{
}

void UWaveSpawnerManagerComponent::BeginPlay()
{
	Super::BeginPlay();
	gatherSpawners();
	UWorld* world = GetWorld();
}

void UWaveSpawnerManagerComponent::gatherSpawners()
{
	TArray<AActor*> FoundActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AWaveSpawner::StaticClass(), FoundActors);
	
	for (AActor* actor : FoundActors)
	{
		AWaveSpawner* waveSpawner = CastChecked<AWaveSpawner>(actor);

		const int32 SpawnGroupIndex = waveSpawner->GetSpawnGroupIndex();
		if (SpawnGroupIndex == AWaveSpawner::INVADLID_SPAWN_GROUP)
		{
			UE_LOG(LogTemp, Warning, TEXT("The SpawnGroup for Wavespawner \"s\" has not been set"),
				*waveSpawner->GetName());
			continue;
		}


		FWaveSpawnerGroup& spawnerGroup = spawnGroups.FindOrAdd(SpawnGroupIndex);
		spawnerGroup.WaveSpawners.Add(waveSpawner);		
	}
}


int32 UWaveSpawnerManagerComponent::StartWave(int32 WaveNumber)
{
	int32 spawnedEnemies = 0;
	const FWaveDefiniton* waveDefiniton = getWaveDefinition(WaveNumber);
	if (waveDefiniton == nullptr)
	{
		return 0;
	}

	const UDataTable* layoutDefinitionTable = waveDefiniton->WaveLayout;
	if (layoutDefinitionTable == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("Layout Definiton was not set for Wave %d"), WaveNumber);
		return 0;
	}

	const TArray<FName> rowNameArray = layoutDefinitionTable->GetRowNames();
	for (const FName& rowName : rowNameArray)
	{
		int32 spawnGroupIndex = FNameToInt(rowName);
		const FWaveLayoutDefiniton* layoutDefiniton = layoutDefinitionTable->FindRow<FWaveLayoutDefiniton>(rowName,
			FString::Printf(TEXT("WaveSpawner Manager, spawnGroup: %d"), spawnGroupIndex));

		spawnedEnemies += spawnForSpawnerGroupIndex(spawnGroupIndex, layoutDefiniton, waveDefiniton);
	}

	return spawnedEnemies;
}

int32 UWaveSpawnerManagerComponent::spawnForSpawnerGroupIndex(int32 SpawnGroupIndex, const FWaveLayoutDefiniton* LayoutDefiniton,
	const FWaveDefiniton* WaveDefinition)
{
	int32 spawnedEnemies = 0;
	FWaveSpawnerGroup* waveSpawnerGroup = spawnGroups.Find(SpawnGroupIndex);
	if (waveSpawnerGroup == nullptr || waveSpawnerGroup->WaveSpawners.Num() <= 0)
	{
		UE_LOG(LogTemp, Error, TEXT("No Wavespawners for group index %d in level!!"), SpawnGroupIndex);
		return spawnedEnemies;
	}

	const int32 waveSpawnerCount = waveSpawnerGroup->WaveSpawners.Num();
	TArray<AWaveSpawner*> waveSpawnerArray = waveSpawnerGroup->WaveSpawners;

	// Add Commands to the spawner's pools
	for (const FSpawnDefiniton& spawnDef : LayoutDefiniton->EnemySpawnDefinitons)
	{
		const int32 TotalEnemyAmount = static_cast<int32>(spawnDef.baseAmount * WaveDefinition->enemyNumberModifier);
		const int32 EnemyAmountPerSpawner = TotalEnemyAmount / waveSpawnerCount;
		const int32 EnemyRemainder = TotalEnemyAmount % waveSpawnerCount;

		spawnedEnemies += TotalEnemyAmount;

		for (int i = 0; i < waveSpawnerCount; ++i)
		{
			// Decides if an additional enemy needs to be added. Can be 0 or 1
			int32 additionalEnemy = EnemyRemainder / (i+1);
			waveSpawnerArray[i]->AddToNextWavePool({ spawnDef.EnemyClass, EnemyAmountPerSpawner + additionalEnemy });
		}
	}


	// Start the spawners
	for (AWaveSpawner* waveSpawner : waveSpawnerArray)
	{
		waveSpawner->SpawnWave(LayoutDefiniton->WaitTimeBeforeSpawning, LayoutDefiniton->SpawningDuration);
	}
	return spawnedEnemies;
}

const FWaveDefiniton* UWaveSpawnerManagerComponent::getWaveDefinition(int32 WaveNumber) const
{
	if (waveDefinitonTable == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("Wave Definition Table is not set"));
		return nullptr;
	}

	return waveDefinitonTable->FindRow<FWaveDefiniton>(IntToFName(WaveNumber),
		FString::Printf(TEXT("WaveSpawner Manager, Wave: %d"), WaveNumber));
}

