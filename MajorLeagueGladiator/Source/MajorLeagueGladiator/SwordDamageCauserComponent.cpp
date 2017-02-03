// Fill out your copyright notice in the Description page of Project Settings.

#include "MajorLeagueGladiator.h"
#include "SwordDamageCauserComponent.h"

USwordDamageCauserComponent::USwordDamageCauserComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	oldSwingSpeed = FVector::ZeroVector;
	threshholdDoDamageSquared = 16;
	threshholdBonusDamageSquared = 36;
}

void USwordDamageCauserComponent::TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction * ThisTickFunction)
{
	FVector newSwordState = GetOwner()->GetVelocity();
	oldSwingSpeed = oldSwingSpeed * 0.9f + DeltaTime * newSwordState * 0.1f;
}

float USwordDamageCauserComponent::CalculateDamage()
{
	return Super::CalculateDamage() * calcBonusDamageFactor();
}

bool USwordDamageCauserComponent::CanDoDamage()
{
	return oldSwingSpeed.SizeSquared() > threshholdDoDamageSquared;
}

float USwordDamageCauserComponent::calcBonusDamageFactor()
{
	return oldSwingSpeed.SizeSquared() > threshholdBonusDamageSquared ? 1.5f : 1.0f;
}
