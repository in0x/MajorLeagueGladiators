// Fill out your copyright notice in the Description page of Project Settings.

#include "Prototype.h"
#include "AmmoPack.h"
#include "TriggerZoneComponent.h"
#include "EventBus.h"
#include <tuple>

AAmmoPack::AAmmoPack()
{
	SetReplicates(true);
}

void AAmmoPack::Use(AActor* CollidingActor, UTriggerZoneComponent* trigger)
{
	if (trigger->GetTriggerType() == TriggerType::Ammo)
	{
		UEventBus::Get().Fire(&UEventBus::AmmoRefillEvent, std::make_tuple(CollidingActor, amountToRefill));
		Destroy();
	}
}


