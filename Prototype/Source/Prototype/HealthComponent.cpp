// Fill out your copyright notice in the Description page of Project Settings.

#include "Prototype.h"
#include "HealthComponent.h"
#include "MessageEndpointBuilder.h"
#include "Messages/MsgHealthRefill.h"

UHealthComponent::UHealthComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	SetIsReplicated(true);
}

void UHealthComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(UHealthComponent, currentHealth);
}

void UHealthComponent::BeginPlay()
{
	msgEndpoint = FMessageEndpoint::Builder("HealthMessager").Handling<FMsgHealthRefill>(this, &UHealthComponent::OnHealthRefill).Build();
	msgEndpoint->Subscribe<FMsgHealthRefill>();
}

void UHealthComponent::OnHealthRefill(const FMsgHealthRefill& Msg, const IMessageContextRef& Context)
{
	if (GetOwner() == Msg.TriggerActor)
	{
		IncreaseHealth(static_cast<float>(Msg.Amount));
	}
}

float UHealthComponent::GetCurrentHealth() const
{
	return currentHealth;
}

float UHealthComponent::GetMaxHealth() const
{
	return maxHealth; 
}

void UHealthComponent::IncreaseHealth(float Val)
{
	checkf(Val > 0, TEXT("Health cannot be decreased by negative value."));
	currentHealth += Val;
	currentHealth = std::min(currentHealth, maxHealth);
}

void UHealthComponent::DecreaseHealth(float Val)
{
	checkf(Val > 0, TEXT("Health cannot be decreased by negative value."));
	currentHealth -= Val;
	currentHealth = std::max(0.f, currentHealth);
}

void UHealthComponent::SetHealthToMax()
{
	currentHealth = maxHealth;
}



