// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "BasePack.h"

#include "AmmoPack.generated.h"

UCLASS()
class MAJORLEAGUEGLADIATOR_API AAmmoPack : public ABasePack
{
	GENERATED_BODY()
public:
	AAmmoPack();

	UFUNCTION()
	virtual void Use(AActor* User, TriggerType Type) override;

private:
	UPROPERTY(EditAnywhere)
	uint32 amountToRefillUncharged;

	UPROPERTY(EditAnywhere)
	uint32 amountToRefillCharged;
};
