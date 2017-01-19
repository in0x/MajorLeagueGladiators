// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "UsableItem.h"
#include "MessageEndpoint.h"
#include "AmmoPack.generated.h"

/**
 * 
 */
UCLASS()
class PROTOTYPE_API AAmmoPack : public AUsableItem
{
	GENERATED_BODY()
public:
	AAmmoPack();

private:
	virtual void Use(AActor* ColidingActor, UTriggerZoneComponent* trigger) override;

	UPROPERTY(EditAnywhere)
	uint32 amountToRefill;

	FMessageEndpointPtr msgEndpoint;
};
