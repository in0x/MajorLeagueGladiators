#pragma once

#include "MessageStructs.generated.h"

USTRUCT() 
struct FAmmoRefillMessage 
{
	GENERATED_USTRUCT_BODY()

	const AActor* TriggerActor;
	int32 Amount;
};