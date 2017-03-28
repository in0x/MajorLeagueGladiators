#pragma once

#include "Engine/DataTable.h"
#include "WaveDefiniton.generated.h"

USTRUCT(BlueprintType)
struct FWaveDefiniton : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()

public:
	UPROPERTY(EditAnywhere)
	UDataTable* WaveLayout;

	UPROPERTY(EditAnywhere)
	float enemyNumberModifier;
};