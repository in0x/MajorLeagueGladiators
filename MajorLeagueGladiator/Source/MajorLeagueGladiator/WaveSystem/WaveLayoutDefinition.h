#pragma once

#include "Engine/DataTable.h"
#include "AI/MlgAICharacter.h"
#include "WaveLayoutDefinition.generated.h"

USTRUCT(BlueprintType)
struct FSpawnDefiniton
{
	GENERATED_USTRUCT_BODY();

public:
	UPROPERTY(EditAnywhere)
	TSubclassOf<AMlgAICharacter> EnemyClass;

	UPROPERTY(EditAnywhere)
	float BaseAmount;
};

USTRUCT(BlueprintType)
struct FWaveLayoutDefiniton : public FTableRowBase
{
	GENERATED_USTRUCT_BODY();
public:
	UPROPERTY(EditAnywhere)
	TArray<FName> SpawnerNames;

	UPROPERTY(EditAnywhere)
	float WaitTimeBeforeSpawning;

	UPROPERTY(EditAnywhere)
	float SpawningDuration;

	UPROPERTY(EditAnywhere)
	TArray<FSpawnDefiniton>	EnemySpawnDefinitons;
};