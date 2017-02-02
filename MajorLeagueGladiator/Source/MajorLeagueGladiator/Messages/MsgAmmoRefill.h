#pragma once

#include "MsgAmmoRefill.generated.h"

USTRUCT() 
struct FMsgAmmoRefill 
{
	GENERATED_USTRUCT_BODY()

	AActor* TriggerActor;
	int32 Amount;
};