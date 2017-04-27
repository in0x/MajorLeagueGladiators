// Fill out your copyright notice in the Description page of Project Settings.

#include "MajorLeagueGladiator.h"
#include "BasePack.h"

#include "PackMovementComponent.h"

namespace
{
	const FName PACK_COLLISION_PROFILE_NAME("Pack");
}

ABasePack::ABasePack(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	SetReplicates(true);
	bReplicateMovement = true;

	MeshComponent->bGenerateOverlapEvents = true;
	MeshComponent->bMultiBodyOverlap = true;
	MeshComponent->SetRelativeScale3D({ 0.25,0.25,0.25 });
	MeshComponent->SetCollisionProfileName(PACK_COLLISION_PROFILE_NAME);

	MeshComponent->SetSimulatePhysics(false);
	movementComponent = ObjectInitializer.CreateDefaultSubobject<UPackMovementComponent>(this, TEXT("PackMovementComponent"));
	movementComponent->bShouldBounce = true;

	static ConstructorHelpers::FObjectFinder<UStaticMesh> packMesh(TEXT("StaticMesh'/Game/MobileStarterContent/Shapes/Shape_Cube.Shape_Cube'"));
	UStaticMeshComponent* staticMeshComp = Cast<UStaticMeshComponent>(MeshComponent);
	if (packMesh.Succeeded() && staticMeshComp)
	{
		staticMeshComp->SetStaticMesh(packMesh.Object);
	}
}


