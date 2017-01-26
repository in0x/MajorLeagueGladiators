// Fill out your copyright notice in the Description page of Project Settings.

#include "Prototype.h"
#include "AmmoComponent.h"
#include "MessageEndpointBuilder.h"
#include "Messages/MsgAmmoRefill.h"

UAmmoComponent::UAmmoComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	SetIsReplicated(true);
}

void UAmmoComponent::BeginPlay()
{
	Super::BeginPlay();
	msgEndpoint = FMessageEndpoint::Builder("AmmoMessager").Handling<FMsgAmmoRefill>(this, &UAmmoComponent::OnAmmoRefill);

	checkf(msgEndpoint.IsValid(), TEXT("Ammo Component Msg Endpoint invalid"));
	msgEndpoint->Subscribe<FMsgAmmoRefill>();
}

int32 UAmmoComponent::GetAmmoCount() const
{
	return static_cast<int32>(ammoCount);
}

int32 UAmmoComponent::GetMaxAmmoCount() const
{
	return static_cast<int32>(maxAmmo);
}

void UAmmoComponent::OnAmmoRefill(const FMsgAmmoRefill& Msg, const IMessageContextRef& Context)
{
	if (GetOwner() == Msg.TriggerActor)
	{
		IncreaseAmmo_NetMulticast(Msg.Amount);
	}
}

void UAmmoComponent::ConsumeAmmo_NetMulticast_Implementation()
{
	ammoCount--;
	OnAmmoChanged.Broadcast(ammoCount);
}

void UAmmoComponent::IncreaseAmmo_NetMulticast_Implementation(uint32 Amount)
{
	ammoCount = FMath::Min(maxAmmo, ammoCount + Amount);
	OnAmmoChanged.Broadcast(ammoCount);
}

TSubclassOf<AGunProjectile> UAmmoComponent::GetProjectileType()
{
	return gunProjectileClass;
}

