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



