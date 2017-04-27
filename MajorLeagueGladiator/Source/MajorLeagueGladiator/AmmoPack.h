// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "UsableItem.h"

#include "AmmoPack.generated.h"

class UPackMovementComponent;

UCLASS()
class MAJORLEAGUEGLADIATOR_API AAmmoPack : public AUsableItem
{
	GENERATED_BODY()
public:
	AAmmoPack();

	UFUNCTION()
	virtual void Use(AActor* User, TriggerType Type) override;

private:
	UPROPERTY(EditAnywhere)
	uint32 amountToRefill;

	UPROPERTY(EditAnywhere)
	UPackMovementComponent* movementComponent;
};
