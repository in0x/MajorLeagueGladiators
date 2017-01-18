// Fill out your copyright notice in the Description page of Project Settings.

#include "Prototype.h"
#include "AmmoComponent.h"
#include "EventBus.h"
#include <tuple>

UAmmoComponent::UAmmoComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UAmmoComponent::BeginPlay()
{
	UEventBus::Get().AmmoRefillEvent.AddLambda([this](const FEventData& data) 
	{
		auto ammoData = data.GetData<std::tuple<AActor*, uint32>>();
	
		if (GetOwner() == std::get<0>(ammoData))
		{
			IncreaseAmmo(std::get<1>(ammoData));
			GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Green, TEXT("Reload"));
		}
	});
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

