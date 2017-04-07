// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Components/ActorComponent.h"
#include "ChargeShotComponent.generated.h"

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class MAJORLEAGUEGLADIATOR_API UChargeShotComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UChargeShotComponent();
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	float ResetAndGetValue();

private:
	float maxValue;
	float currentValue;
	float minValue;

	float timeToReachMaxValue;
	float currentTime;
};
