// Fill out your copyright notice in the Description page of Project Settings.

#include "Prototype.h"
#include "AmmoPack.h"
#include "TriggerZoneComponent.h"
#include "MessageStructs.h"
#include "MessageEndpointBuilder.h"

AAmmoPack::AAmmoPack()
{
	SetReplicates(true);
	msgEndpoint = FMessageEndpoint::Builder("AmmoPackMessager").Build();
}

void AAmmoPack::Use(AActor* CollidingActor, UTriggerZoneComponent* trigger)
{
	if (trigger->GetTriggerType() == TriggerType::Ammo)
	{
		FAmmoRefillMessage msg = {CollidingActor, amountToRefill};
		msgEndpoint->Publish<FAmmoRefillMessage>(&msg);
		Destroy();
	}
}


