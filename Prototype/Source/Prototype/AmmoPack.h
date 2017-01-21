// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "UsableItem.h"
#include "MessageEndpoint.h"
#include "Usable.h"
#include "AmmoPack.generated.h"

/**
 * 
 */
UCLASS()
class PROTOTYPE_API AAmmoPack : public AUsableItem, public IUsable
{
	GENERATED_BODY()
public:
	AAmmoPack();
	void BeginPlay();

	UFUNCTION()
	virtual void IUse(AActor* user, TriggerType triggerType) override;

private:
	virtual void Use(AActor* ColidingActor, UTriggerZoneComponent* trigger) override;

	UPROPERTY(EditAnywhere)
	uint32 amountToRefill;
		
	FMessageEndpointPtr msgEndpoint;
};
