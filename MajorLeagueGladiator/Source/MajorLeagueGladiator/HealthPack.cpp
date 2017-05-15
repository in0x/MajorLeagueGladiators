// Fill out your copyright notice in the Description page of Project Settings.

#include "MajorLeagueGladiator.h"
#include "HealthPack.h"
#include "TriggerZoneComponent.h"
#include "HealthComponent.h"
#include "Characters/RangedPlayerCharacter.h"

AHealthPack::AHealthPack()
	: amountToRefillUncharged(30.f)
	, amountToRefillCharged(100.f)
{
	ConstructorHelpers::FObjectFinder<UMaterialInterface> healthPackMat(TEXT("Material'/Game/BluePrints/HealthPackMat.HealthPackMat'"));

	UStaticMeshComponent* staticMeshComp = Cast<UStaticMeshComponent>(MeshComponent);
	if (staticMeshComp && healthPackMat.Succeeded())
	{
		staticMeshComp->SetMaterial(0, healthPackMat.Object);
	}
	chargingPlayerClass = ARangedPlayerCharacter::StaticClass();
}

void AHealthPack::Use(AActor* User, TriggerType Type)
{
	if (Type != TriggerType::Health)
	{
		return;
	}
	
	UHealthComponent* healthComponent = User->FindComponentByClass<UHealthComponent>();

	if (!healthComponent)
	{
		auto* owner = User->GetOwner();
		if (owner)
		{
			healthComponent = owner->FindComponentByClass<UHealthComponent>();
		}
	}
		
	if (!healthComponent)
	{
		UE_LOG(DebugLog, Warning, TEXT("Owner of health trigger has no healthcomponent, cannot use healthpack."));
		return;
	}

	if (healthComponent->GetCurrentHealthPercentage() < 1.f)
	{
		const float amoutToRefill = IsCharged() ? amountToRefillCharged : amountToRefillUncharged;
		healthComponent->IncreaseHealth(amoutToRefill);

		ReleaseFromGrippedComponent();
		Destroy();
	}
}
