#pragma once

#include "MessageStructs.generated.h"

USTRUCT() 
struct FAmmoRefillMessage 
{
	GENERATED_USTRUCT_BODY()

	AActor* TriggerActor;
	int32 Amount;
};

USTRUCT()
struct FHealthRefillMsg
{
	GENERATED_USTRUCT_BODY()

	AActor* TriggerActor;
	int32 Amount;
};