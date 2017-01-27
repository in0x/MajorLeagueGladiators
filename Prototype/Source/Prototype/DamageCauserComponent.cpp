// Fill out your copyright notice in the Description page of Project Settings.

#include "Prototype.h"
#include "DamageCauserComponent.h"

UDamageCauserComponent::UDamageCauserComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	bCanCauseDamage = true;
}

void UDamageCauserComponent::BeginPlay()
{
	Super::BeginPlay();
	GetOwner()->OnActorBeginOverlap.AddDynamic(this, &UDamageCauserComponent::OnBeginOverlap);
}

void UDamageCauserComponent::OnBeginOverlap(AActor* OverlappedActor, AActor* OtherActor)
{
	UGameplayStatics::ApplyDamage(OtherActor, damageAppliedOnHit, nullptr, OverlappedActor, damageType);
}

void UDamageCauserComponent::onCooldownReset()
{
	bCanCauseDamage = true;
}
