// Fill out your copyright notice in the Description page of Project Settings.

#include "MajorLeagueGladiator.h"
#include "AmmoPack.h"
#include "TriggerZoneComponent.h"
#include "AmmoComponent.h"

namespace
{
	const char* PACK_COLLISION_PROFILE_NAME = "Pack";
}

AAmmoPack::AAmmoPack()
{
	SetReplicates(true);

	MeshComponent->bGenerateOverlapEvents = true;
	MeshComponent->bMultiBodyOverlap = true;
	MeshComponent->SetRelativeScale3D({ 0.25,0.25,0.25 });
	MeshComponent->SetSimulatePhysics(true);

	static ConstructorHelpers::FObjectFinder<UStaticMesh> ammoPackMesh(TEXT("StaticMesh'/Game/MobileStarterContent/Shapes/Shape_Cube.Shape_Cube'"));
	UStaticMeshComponent* staticMeshComp = Cast<UStaticMeshComponent>(MeshComponent);
	if (ammoPackMesh.Succeeded() && staticMeshComp)
	{
		staticMeshComp->SetStaticMesh(ammoPackMesh.Object);

		ConstructorHelpers::FObjectFinder<UMaterialInterface> ammoPackMat(TEXT("Material'/Game/BluePrints/AmmoPackMat.AmmoPackMat'"));
		staticMeshComp->SetMaterial(0, ammoPackMat.Object);
	}
	bReplicateMovement = true;
	MeshComponent->SetCollisionProfileName(PACK_COLLISION_PROFILE_NAME);
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

