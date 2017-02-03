// Fill out your copyright notice in the Description page of Project Settings.

#include "MajorLeagueGladiator.h"
#include "SwordDamageCauserComponent.h"

USwordDamageCauserComponent::USwordDamageCauserComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	oldSwingSpeed = FVector::ZeroVector;
	threshholdDoDamageSquared = 16;
	threshholdBonusDamageSquared = 36;
	bonusDamageFactor = 1.5f;
	slashDetectionAlpha = 0.1f;
}

void USwordDamageCauserComponent::TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	FVector newSwordState = GetOwner()->GetVelocity();
	oldSwingSpeed = oldSwingSpeed * (1.0f - slashDetectionAlpha) + DeltaTime * (newSwordState * slashDetectionAlpha);
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
	return oldSwingSpeed.SizeSquared() > threshholdBonusDamageSquared ? bonusDamageFactor : 1.0f;
}
