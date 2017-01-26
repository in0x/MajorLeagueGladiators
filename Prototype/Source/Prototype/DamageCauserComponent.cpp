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
	GetOwner()->OnActorHit.AddDynamic(this, &UDamageCauserComponent::OnHit);
}

void UDamageCauserComponent::OnHit(AActor* SelfActor, AActor* OtherActor, FVector NormalImpulse, const FHitResult& Hit)
{
	if (!bCanCauseDamage || !OtherActor->bCanBeDamaged)
		return;

	bCanCauseDamage = false;
	GetOwner()->GetWorldTimerManager().SetTimer(cooldownTimerHandle, this, &UDamageCauserComponent::onCooldownReset, timeBeforeCanDamageAgainSeconds, false);

	UGameplayStatics::ApplyDamage(OtherActor, damageAppliedOnHit, nullptr, SelfActor, damageType);
}

void UDamageCauserComponent::onCooldownReset()
{
	bCanCauseDamage = true;
}
