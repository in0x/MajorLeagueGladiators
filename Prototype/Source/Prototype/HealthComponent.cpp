// Fill out your copyright notice in the Description page of Project Settings.

#include "Prototype.h"
#include "HealthComponent.h"

UHealthComponent::UHealthComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

float UHealthComponent::CurrentHealth() const
{
	return currentHealth;
}

float UHealthComponent::MaxHealth() const
{
	return maxHealth;
}

void UHealthComponent::DecreaseHealth(float Val)
{
	Val = std::abs(Val);
	currentHealth -= Val;
	currentHealth = std::max(0.f, currentHealth);
}

void UHealthComponent::RefillHealth()
{
	currentHealth = maxHealth;
}



