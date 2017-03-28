#pragma once

#include "WaveSpawnerGroup.generated.h"

class AWaveSpawner;

USTRUCT()
struct FWaveSpawnerGroup
{
	GENERATED_USTRUCT_BODY();
public:
	
	TArray<AWaveSpawner*> WaveSpawners;
	
};
