// Fill out your copyright notice in the Description page of Project Settings.

#include "MajorLeagueGladiator.h"
#include "AmmoComponent.h"
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

void UAmmoComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(UAmmoComponent, ammoCount);
}

void UAmmoComponent::onRep_ammoCount()
{
	OnAmmoChanged.Broadcast(ammoCount);
}

int32 UAmmoComponent::GetAmmoCount() const
{
	return ammoCount;
}

int32 UAmmoComponent::GetMaxAmmoCount() const
{
	return maxAmmo;
}

void UAmmoComponent::OnAmmoRefill(const FMsgAmmoRefill& Msg, const IMessageContextRef& Context)
{
	if (GetOwner() == Msg.TriggerActor)
	{
		IncreaseAmmo(Msg.Amount);
	}
}

void UAmmoComponent::ConsumeAmmo()
{
	if (ammoCount == 0)
		return;

	ammoCount--;
	OnAmmoChanged.Broadcast(ammoCount);
}

void UAmmoComponent::IncreaseAmmo(int32 Amount)
{
	ammoCount = FMath::Min(maxAmmo, ammoCount + Amount);
	OnAmmoChanged.Broadcast(ammoCount);
}
