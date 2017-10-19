// Fill out your copyright notice in the Description page of Project Settings.

#include "MajorLeagueGladiator.h"
#include "ChargeShotComponent.h"
#include "Projectiles/HitscanProjectile.h"

UChargeShotComponent::UChargeShotComponent()
	: MaxValue(2.0f)
	, MinValue(0.5f)
	, TimeToReachMaxValue(3.0f)
	, currentValue(0.0f)
	, currentTime(0.0f)
	, projectileClass(AHitscanProjectile::StaticClass())
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

void UChargeShotComponent::FireHitscanShot()
{
	// This component should be attached onto the socket we want to shoot from
	FTransform transform = GetComponentTransform();

	float charge = GetValue();

	FProjectileSpawnParams params;
	params.DamageScale = charge;
	params.Scale3DMultiplier = FVector(charge, charge, 1.f);

	ABaseProjectile* defaultProjectile = projectileClass.GetDefaultObject();

	defaultProjectile->FireProjectile(transform.GetLocation(), transform.GetRotation().GetForwardVector(), GetOwner(), GetOwner()->GetInstigatorController(), params);

	Reset();
}

void UChargeShotComponent::Reset()
{
	currentTime = 0.f;
}


