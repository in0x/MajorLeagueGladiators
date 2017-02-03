// Fill out your copyright notice in the Description page of Project Settings.

#include "MajorLeagueGladiator.h"
#include "DamageCauserComponent.h"

UDamageCauserComponent::UDamageCauserComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UDamageCauserComponent::BeginPlay()
{
	Super::BeginPlay();
	GetOwner()->OnActorBeginOverlap.AddDynamic(this, &UDamageCauserComponent::OnBeginOverlap);
}

void UDamageCauserComponent::OnBeginOverlap(AActor* OverlappedActor, AActor* OtherActor)
{
	if (CanDoDamage())
	{
		float damage = CalculateDamage();
		UGameplayStatics::ApplyDamage(OtherActor, damage, nullptr, OverlappedActor, damageType);
	}
}

bool UDamageCauserComponent::CanDoDamage()
{
	return true;
}

float UDamageCauserComponent::CalculateDamage()
{
	return damageAppliedOnHit;
}
