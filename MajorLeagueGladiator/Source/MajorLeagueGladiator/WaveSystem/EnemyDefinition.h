#pragma once

#include "Engine/DataTable.h"
#include "EnemyDefinition.generated.h"

USTRUCT(BlueprintType)
struct FEnemyDefinition : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()

public:
	UPROPERTY()
	TSubclassOf<AActor> enemyClass;
};