// Fill out your copyright notice in the Description page of Project Settings.

#include "MajorLeagueGladiator.h"
#include "HealthPack.h"
#include "TriggerZoneComponent.h"
#include "HealthComponent.h"

AHealthPack::AHealthPack()
{
	SetReplicates(true);
	bStaticMeshReplicateMovement = true;
	auto mesh = GetStaticMeshComponent();

	if (mesh)
	{
		mesh->bGenerateOverlapEvents = true;
		mesh->bMultiBodyOverlap = true;
	}
}

void AHealthPack::Use(AActor* User, TriggerType Type)
{
	if (Type == TriggerType::Health)
	{
		UHealthComponent* healthComponent = User->GetOwner()->FindComponentByClass<UHealthComponent>();
		
		if (!healthComponent)
		{
			UE_LOG(DebugLog, Warning, TEXT("Owner of health trigger has no healthcomponent, cannot use healthpack."));
			return;
		}

		if (healthComponent->GetCurrentHealthPercentage() < 1.f)
		{
			healthComponent->IncreaseHealth(amountToRefill);
			ReleaseFromGrippedComponent();
			Destroy();
		}
	}
}
