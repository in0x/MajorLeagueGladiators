// Fill out your copyright notice in the Description page of Project Settings.

#include "MajorLeagueGladiator.h"
#include "SwordDamageCauserComponent.h"

USwordDamageCauserComponent::USwordDamageCauserComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	bCanCauseDamage = false;
	damageScaleFactor = 1;
	oldSwingSpeed = FVector::ZeroVector;
}

void USwordDamageCauserComponent::TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction * ThisTickFunction)
{
	FVector newSwordState = GetOwner()->GetVelocity();
	oldSwingSpeed = oldSwingSpeed * 0.9f + DeltaTime * newSwordState * 0.1f;
}

void USwordDamageCauserComponent::DoOverlapAction(AActor* OverlappedActor, AActor* OtherActor)
{
	if (oldSwingSpeed.Size() > 4)
	{
		UGameplayStatics::ApplyDamage(OtherActor, damageAppliedOnHit, nullptr, OverlappedActor, damageType);
	}
}

