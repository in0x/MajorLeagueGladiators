// Fill out your copyright notice in the Description page of Project Settings.

#include "Prototype.h"
#include "AmmoComponent.h"

UAmmoComponent::UAmmoComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UAmmoComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UAmmoComponent::TickComponent( float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction )
{
	Super::TickComponent( DeltaTime, TickType, ThisTickFunction );
}

bool UAmmoComponent::PerformShot()
{
	GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Green, FString::Printf(TEXT("Ammo: %d / %d"), ammoCount, maxAmmo));

	if (ammoCount == 0)
		return false;

	ammoCount--;
	return true;
}

void UAmmoComponent::IncreaseAmmoCount(uint32 Amount)
{
	if (ammoCount == maxAmmo)
		return;

	ammoCount = FMath::Min(maxAmmo, ammoCount + Amount);
}

TSubclassOf<AGunProjectile> UAmmoComponent::GetProjectileType()
{
	return gunProjectileClass;
}

