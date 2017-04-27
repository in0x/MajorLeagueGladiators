// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "UsableItem.h"
#include "BasePack.generated.h"

class UPackMovementComponent;

UCLASS(Abstract)
class MAJORLEAGUEGLADIATOR_API ABasePack : public AUsableItem
{
	GENERATED_BODY()

public:		
	ABasePack(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
	
private:
	UPROPERTY(EditAnywhere)
	UPackMovementComponent* movementComponent;
};
