#pragma once

#include "Engine/DataTable.h"
#include "WaveLayoutDefinition.generated.h"

USTRUCT(BlueprintType)
struct FSpawnDefiniton
{
	GENERATED_USTRUCT_BODY();

public:
	UPROPERTY(EditAnywhere)
	TSubclassOf<ACharacter> EnemyClass;

	UPROPERTY(EditAnywhere)
	float baseAmount;
};

USTRUCT(BlueprintType)
struct FWaveLayoutDefiniton : public FTableRowBase
{
	GENERATED_USTRUCT_BODY();
public:

	UPROPERTY(EditAnywhere)
	float WaitTimeBeforeSpawning;

	UPROPERTY(EditAnywhere)
	float SpawningDuration;

	UPROPERTY(EditAnywhere)
	TArray<FSpawnDefiniton>	EnemySpawnDefinitons;
};