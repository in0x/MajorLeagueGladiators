// Fill out your copyright notice in the Description page of Project Settings.

#include "MajorLeagueGladiator.h"
#include "HealthComponent.h"
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
	if (GetOwner() == Msg.TriggerActor || Msg.TriggerActor->IsAttachedTo(GetOwner()))
	{
		IncreaseHealth(static_cast<float>(Msg.Amount));
	}
}

void UHealthComponent::onRep_CurrentHealth(float oldHealth)
{
	HealthChangedDelegate.Broadcast(currentHealth / maxHealth, oldHealth / maxHealth);
}

float UHealthComponent::GetCurrentHealth() const
{
	return currentHealth;
}

float UHealthComponent::GetCurrentHealthPercentage() const
{
	return currentHealth / maxHealth;
}

float UHealthComponent::GetMaxHealth() const
{
	return maxHealth; 
}

void UHealthComponent::IncreaseHealth(float Val)
{
	checkf(Val > 0, TEXT("Health cannot be decreased by negative value."));
	float oldHealthPercentage = GetCurrentHealthPercentage();
	currentHealth += Val;
	currentHealth = std::min(currentHealth, maxHealth);
	HealthChangedDelegate.Broadcast(GetCurrentHealthPercentage(), oldHealthPercentage);
}

void UHealthComponent::DecreaseHealth(float Val)
{
	checkf(Val > 0, TEXT("Health cannot be decreased by negative value."));
	float oldHealthPercentage = GetCurrentHealthPercentage();
	currentHealth -= Val;
	currentHealth = std::max(0.f, currentHealth);
	HealthChangedDelegate.Broadcast(GetCurrentHealthPercentage(), oldHealthPercentage);
}

void UHealthComponent::SetHealthToMax()
{
	float oldHealthPercentage = GetCurrentHealthPercentage();
	currentHealth = maxHealth;
	HealthChangedDelegate.Broadcast(1.f, oldHealthPercentage);
}



