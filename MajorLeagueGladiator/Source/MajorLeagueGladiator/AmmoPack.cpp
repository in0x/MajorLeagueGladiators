// Fill out your copyright notice in the Description page of Project Settings.

#include "MajorLeagueGladiator.h"
#include "AmmoPack.h"
#include "TriggerZoneComponent.h"
#include "AmmoComponent.h"

AAmmoPack::AAmmoPack()
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

void AAmmoPack::Use(AActor* User, TriggerType Type)
{
	UAmmoComponent* ammoComponent = User->FindComponentByClass<UAmmoComponent>();

	if (!ammoComponent)
	{
		ammoComponent = User->GetOwner()->FindComponentByClass<UAmmoComponent>();
	}

	if (!ammoComponent)
	{
		UE_LOG(DebugLog, Warning, TEXT("Owner of ammo trigger has no healthcomponent, cannot use ammopack."));
		return;
	}

	if (ammoComponent->GetAmmoCount() < ammoComponent->GetMaxAmmoCount())
	{
		ammoComponent->IncreaseAmmo(amountToRefill);
		ReleaseFromGrippedComponent();
		Destroy();
	}
}

