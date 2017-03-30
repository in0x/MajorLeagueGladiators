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

		const FName& uniqueName = waveSpawner->GetUniqueName();


		if (uniqueName == AWaveSpawner::INVADLID_NAME)
		{
			UE_LOG(LogTemp, Warning, TEXT("The uniqueName for Wavespawner \"%s\" has not been set"),
				*waveSpawner->GetName());
			continue;
		}

		if(waveSpawners.Contains(uniqueName))
		{
			UE_LOG(LogTemp, Warning, TEXT("The for Wavespawner \"%s\" is not unique"),
				*waveSpawner->GetName());
		}
		
		waveSpawners.Add(uniqueName, waveSpawner);
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

	TArray<FWaveLayoutDefiniton*> outLayoutDefinitions;
	layoutDefinitionTable->GetAllRows(TEXT("WaveSpawner Manager"), outLayoutDefinitions);
	for (const FWaveLayoutDefiniton* waveLayout : outLayoutDefinitions)
	{

		spawnedEnemies += spawnForLayout(waveLayout, waveDefiniton);
	}

	return spawnedEnemies;
}

int32 UWaveSpawnerManagerComponent::spawnForLayout(const FWaveLayoutDefiniton* LayoutDefiniton, const FWaveDefiniton* WaveDefinition)
{
	const TArray<FName>& waveSpawnerNames = LayoutDefiniton->SpawnerNames;
	const int32 waveSpawnerCount = waveSpawnerNames.Num();
	if (waveSpawnerCount == 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("No Wavespawners set for Layout"));
		return 0;
	}
	
	const TArray<FSpawnDefiniton>& spawnDefArray = LayoutDefiniton->EnemySpawnDefinitons;

	TArray<FSpawnCommand> spawnCommandArray;
	spawnCommandArray.Reserve(spawnDefArray.Num());

	const float spawnMultipier = WaveDefinition->EnemyNumberModifier;

	for (const FSpawnDefiniton& spawnDef : spawnDefArray)
	{
		const int32 numberToSpawn = static_cast<int32>(spawnDef.BaseAmount * spawnMultipier);
		spawnCommandArray.Add({ spawnDef.EnemyClass, numberToSpawn });
	}

	int32 spawnedEnemies = 0;

	TArray<AWaveSpawner*> waveSpawnerArray = FindWaveSpawnersByNames(waveSpawnerNames);
	for (AWaveSpawner* waveSpawner : waveSpawnerArray)
	{
		for (const FSpawnCommand& spawnCommand : spawnCommandArray)
		{
			waveSpawner->AddToNextWavePool(spawnCommand);
			spawnedEnemies += spawnCommand.EnemyCount;
		}
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

TArray<AWaveSpawner*> UWaveSpawnerManagerComponent::FindWaveSpawnersByNames(const TArray<FName>& Names)
{
	TArray<AWaveSpawner*> foundWaveSpawners;
	foundWaveSpawners.Reserve(Names.Num());
	for (const FName& name : Names)
	{
		AWaveSpawner** spawner = waveSpawners.Find(name);
		if (spawner == nullptr)
		{
			UE_LOG(LogTemp, Warning, TEXT("WaveSpawner \"%s\" not found in level"), *name.ToString());
			continue;
		}
		foundWaveSpawners.Add(*spawner);
	}
	return foundWaveSpawners;
}
