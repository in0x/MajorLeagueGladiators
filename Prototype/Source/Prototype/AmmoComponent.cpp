// Fill out your copyright notice in the Description page of Project Settings.

#include "Prototype.h"
#include "AmmoComponent.h"
#include "MessageEndpointBuilder.h"

UAmmoComponent::UAmmoComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	msgEndpoint = FMessageEndpoint::Builder("AmmoMessager").Handling<FAmmoRefillMessage>(this, &UAmmoComponent::OnAmmoRefill).Build();
	msgEndpoint->Subscribe<FAmmoRefillMessage>();
}

void UAmmoComponent::OnAmmoRefill(const FAmmoRefillMessage& Msg, const IMessageContextRef& Context)
{
	if (GetOwner() == Msg.TriggerActor)
	{
		IncreaseAmmo(Msg.Amount);
		GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Green, TEXT("Reload"));
	}
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

