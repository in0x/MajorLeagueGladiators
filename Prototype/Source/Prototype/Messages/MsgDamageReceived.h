#pragma once

#include "MsgDamageReceived.generated.h"

USTRUCT()
struct FMsgDamageReceived
{
	GENERATED_USTRUCT_BODY()

	AActor* DamagedActor;
	float Damage;
	const UDamageType* DamageType;
	AController* InstigatedBy;
	AActor* DamageCauser;
};