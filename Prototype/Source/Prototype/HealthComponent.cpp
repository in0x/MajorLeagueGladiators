// Fill out your copyright notice in the Description page of Project Settings.

#include "Prototype.h"
#include "HealthComponent.h"

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

void UHealthComponent::onRep_CurrentHealth()
{
	HealthChangedDelegate.Broadcast(currentHealth / maxHealth);
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
	currentHealth += Val;
	currentHealth = std::min(currentHealth, maxHealth);
	HealthChangedDelegate.Broadcast(GetCurrentHealthPercentage());
}

void UHealthComponent::DecreaseHealth(float Val)
{
	checkf(Val > 0, TEXT("Health cannot be decreased by negative value."));
	currentHealth -= Val;
	currentHealth = std::max(0.f, currentHealth);
	HealthChangedDelegate.Broadcast(GetCurrentHealthPercentage());
}

void UHealthComponent::SetHealthToMax()
{
	currentHealth = maxHealth;
	HealthChangedDelegate.Broadcast(1.f);
}



