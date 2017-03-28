#pragma once

#include "SpawnCommand.generated.h"

USTRUCT()
struct FSpawnCommand
{
	GENERATED_USTRUCT_BODY();
public:
	TSubclassOf<ACharacter> enemyClass;
	int enemyCount;
};