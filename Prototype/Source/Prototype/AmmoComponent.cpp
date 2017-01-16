// Fill out your copyright notice in the Description page of Project Settings.

#include "Prototype.h"
#include "AmmoComponent.h"

UAmmoComponent::UAmmoComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

bool UAmmoComponent::ConsumeAmmo()
{
	GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Green, FString::Printf(TEXT("Ammo: %d / %d"), ammoCount, maxAmmo));

	if (ammoCount == 0)
		return false;

	ammoCount--;
	return true;
}

void UAmmoComponent::IncreaseAmmo(uint32 Amount)
{
	ammoCount = FMath::Min(maxAmmo, ammoCount + Amount);
}

TSubclassOf<AGunProjectile> UAmmoComponent::GetProjectileType()
{
	return gunProjectileClass;
}

