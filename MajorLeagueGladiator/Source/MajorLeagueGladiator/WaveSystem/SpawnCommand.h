#pragma once

#include "AI/MlgAICharacter.h"
#include "SpawnCommand.generated.h"

USTRUCT()
struct FSpawnCommand
{
	GENERATED_USTRUCT_BODY();
public:

	TSubclassOf<AMlgAICharacter> EnemyClass;
	int32 EnemyCount;
};