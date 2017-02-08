// Fill out your copyright notice in the Description page of Project Settings.

#include "MajorLeagueGladiator.h"
#include "DamageCauserComponent.h"
#include "MlgGrippableStaticMeshActor.h"
#include "MlgPlayerController.h"

UDamageCauserComponent::UDamageCauserComponent()
	: damageAppliedOnHit(0)
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
		AMlgGrippableStaticMeshActor* owner = Cast<AMlgGrippableStaticMeshActor>(GetOwner());
		if (owner != nullptr) 
		{
			AMlgPlayerController* controller = owner->getCurrentGrippedController();
			if (controller != nullptr) 
			{
				controller->ClientPlayForceFeedback(controller->rumbleTest, false, FName());
			}
		}
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
