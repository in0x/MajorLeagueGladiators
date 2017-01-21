// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "UsableItem.h"
#include "MessageEndpoint.h"
#include "AmmoPack.generated.h"

UCLASS()
class PROTOTYPE_API AAmmoPack : public AUsableItem
{
	GENERATED_BODY()
public:
	AAmmoPack();
	void BeginPlay();

	UFUNCTION()
	virtual void Use(AActor* user, TriggerType triggerType) override;

private:
	UPROPERTY(EditAnywhere)
	uint32 amountToRefill;
		
	FMessageEndpointPtr msgEndpoint;
};
