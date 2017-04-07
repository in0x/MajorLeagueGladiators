// Fill out your copyright notice in the Description page of Project Settings.

#include "MajorLeagueGladiator.h"
#include "ChargeShotComponent.h"

UChargeShotComponent::UChargeShotComponent()
	: maxValue(2.0f)
	, currentValue(0.0f)
	, minValue(0.5f)
	, timeToReachMaxValue(3.0f)
	, currentTime(0.0f)
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UChargeShotComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UChargeShotComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (currentTime < timeToReachMaxValue)
	{
		currentTime += DeltaTime;
		currentTime = FMath::Min(currentTime, timeToReachMaxValue);
		currentValue = FMath::Lerp(minValue, maxValue, currentTime / timeToReachMaxValue);
		OnChargeValueChangedPercentage.Broadcast(currentValue / maxValue);
	}
}

float UChargeShotComponent::GetValueAndReset()
{
	currentTime = 0.f;
	return currentValue;
}


