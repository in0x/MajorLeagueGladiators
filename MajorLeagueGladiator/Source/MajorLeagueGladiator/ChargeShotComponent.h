// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Components/ActorComponent.h"
#include "ChargeShotComponent.generated.h"

DECLARE_MULTICAST_DELEGATE_OneParam(ChargeValueChangedPercentageDelegate, float);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class MAJORLEAGUEGLADIATOR_API UChargeShotComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UChargeShotComponent();
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void Reset();
	float GetValue();
	float GetValueAndReset();
	float GetValuePercentage();

	ChargeValueChangedPercentageDelegate OnChargeValueChangedPercentage;

	float MaxValue;
	float MinValue;
	float TimeToReachMaxValue;

private:
	float currentValue;
	float currentTime;
};
