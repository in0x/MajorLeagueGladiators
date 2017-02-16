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
		UGameplayStatics::ApplyDamage(OtherActor, damageAppliedOnHit, nullptr, OverlappedActor, damageType);
		doRumbleRight(OtherActor);
	}
}

bool UDamageCauserComponent::CanDoDamage()
{
	return true;
}

void UDamageCauserComponent::doRumbleRight(AActor* OtherActor)
{
	AMlgGrippableStaticMeshActor* owner = Cast<AMlgGrippableStaticMeshActor>(GetOwner());
	if (owner != nullptr)
	{
		AMlgPlayerController* controller = owner->GetMlgPlayerController();
		if (controller != nullptr)
		{
			//check if hit object is part of myself, then we do not rumble
			APawn* otherPlayer = Cast<APawn>(OtherActor);
			if (otherPlayer != nullptr)
			{
				if (otherPlayer->Controller == controller)
				{
					return;
				}
			}
			controller->ClientPlayForceFeedback(controller->GetRumbleShortRight(), false, FName("rumbleRight"));
		}
	}
}