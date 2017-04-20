// Fill out your copyright notice in the Description page of Project Settings.

#include "MajorLeagueGladiator.h"
#include "HealthPack.h"
#include "TriggerZoneComponent.h"
#include "HealthComponent.h"

namespace
{
	const char* PACK_COLLISION_PROFILE_NAME = "Pack";
}

AHealthPack::AHealthPack()
{
	SetReplicates(true);

	MeshComponent->bGenerateOverlapEvents = true;
	MeshComponent->bMultiBodyOverlap = true;
	MeshComponent->SetRelativeScale3D({ 0.25,0.25,0.25 });
	MeshComponent->SetSimulatePhysics(true);

	static ConstructorHelpers::FObjectFinder<UStaticMesh> healthPackMesh(TEXT("StaticMesh'/Game/MobileStarterContent/Shapes/Shape_Cube.Shape_Cube'"));
	UStaticMeshComponent* staticMeshComp = Cast<UStaticMeshComponent>(MeshComponent);
	if (healthPackMesh.Succeeded() && staticMeshComp)
	{
		staticMeshComp->SetStaticMesh(healthPackMesh.Object);
		ConstructorHelpers::FObjectFinder<UMaterialInterface> healthPackMat(TEXT("Material'/Game/BluePrints/HealthPackMat.HealthPackMat'"));
		staticMeshComp->SetMaterial(0, healthPackMat.Object);
	}

	bReplicateMovement = true;
	MeshComponent->SetCollisionProfileName(PACK_COLLISION_PROFILE_NAME);
	
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
		healthComponent->IncreaseHealth(amountToRefill);
		ReleaseFromGrippedComponent();
		Destroy();
	}
}
