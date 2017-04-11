// Fill out your copyright notice in the Description page of Project Settings.

#include "MajorLeagueGladiator.h"
#include "ChargeShotComponent.h"

UChargeShotComponent::UChargeShotComponent()
	: MaxValue(2.0f)
	, MinValue(0.5f)
	, TimeToReachMaxValue(3.0f)
	, currentValue(0.0f)
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

	if (currentTime < TimeToReachMaxValue)
	{
		currentTime += DeltaTime;
		currentTime = FMath::Min(currentTime, TimeToReachMaxValue);
		currentValue = FMath::Lerp(MinValue, MaxValue, currentTime / TimeToReachMaxValue);
		OnChargeValueChangedPercentage.Broadcast(GetValuePercentage());
	}
}

float UChargeShotComponent::GetValue()
{
	return currentValue;
}

float UChargeShotComponent::GetValuePercentage()
{
	return currentValue / MaxValue;
}

void UChargeShotComponent::Reset()
{
	currentTime = 0.f;
}


