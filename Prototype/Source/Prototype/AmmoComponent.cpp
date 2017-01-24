// Fill out your copyright notice in the Description page of Project Settings.

#include "Prototype.h"
#include "AmmoComponent.h"
#include "MessageEndpointBuilder.h"
#include "Messages/MsgAmmoRefill.h"

UAmmoComponent::UAmmoComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UAmmoComponent::BeginPlay()
{
	Super::BeginPlay();
	msgEndpoint = FMessageEndpoint::Builder("AmmoMessager").Handling<FMsgAmmoRefill>(this, &UAmmoComponent::OnAmmoRefill);

	checkf(msgEndpoint.IsValid(), TEXT("Ammo Component Msg Endpoint invalid"));
	msgEndpoint->Subscribe<FMsgAmmoRefill>();
}

void UAmmoComponent::OnAmmoRefill(const FMsgAmmoRefill& Msg, const IMessageContextRef& Context)
{
	if (GetOwner() == Msg.TriggerActor)
	{
		IncreaseAmmo(Msg.Amount);
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

