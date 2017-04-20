// Fill out your copyright notice in the Description page of Project Settings.

#include "MajorLeagueGladiator.h"
#include "AmmoPack.h"
#include "TriggerZoneComponent.h"
#include "AmmoComponent.h"

AAmmoPack::AAmmoPack()
{
	SetReplicates(true);

	MeshComponent->bGenerateOverlapEvents = true;
	MeshComponent->bMultiBodyOverlap = true;
}

void AAmmoPack::Use(AActor* User, TriggerType Type)
{
	if (Type != TriggerType::Ammo)
	{
		return;
	}

	UAmmoComponent* ammoComponent = User->FindComponentByClass<UAmmoComponent>();

	if (!ammoComponent)
	{
		auto* owner = User->GetOwner();
		if (owner) 
		{
			ammoComponent = owner->FindComponentByClass<UAmmoComponent>();
		}
	}

	if (!ammoComponent)
	{
		UE_LOG(DebugLog, Warning, TEXT("Owner of ammo trigger has no ammocomponent, cannot use ammopack."));
		return;
	}

	if (ammoComponent->GetAmmoCount() < ammoComponent->GetMaxAmmoCount())
	{
		ammoComponent->IncreaseAmmo(amountToRefill);
		ReleaseFromGrippedComponent();
		Destroy();
	}
}

