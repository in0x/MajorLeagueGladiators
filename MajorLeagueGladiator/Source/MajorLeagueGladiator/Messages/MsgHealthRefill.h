#pragma once

#include "MsgHealthRefill.generated.h"

USTRUCT()
struct FMsgHealthRefill
{
	GENERATED_USTRUCT_BODY()

	AActor* TriggerActor;
	float Amount;
};