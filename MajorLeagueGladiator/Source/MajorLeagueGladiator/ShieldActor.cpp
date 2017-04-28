// Fill out your copyright notice in the Description page of Project Settings.

#include "MajorLeagueGladiator.h"
#include "ShieldActor.h"

#include "Projectiles/BaseProjectile.h"
#include "MlgPlayerController.h"

namespace
{
	const FName REFLECT_SOCKET_NAME("Reflect");
	const FName SHIELD_COLLISION_PRESET_NAME("Shield");
}

AShieldActor::AShieldActor(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer.SetDefaultSubobjectClass<UStaticMeshComponent>(Super::MESH_COMPONENT_NAME))
{
	bReplicates = true;
	MeshComponent->bGenerateOverlapEvents = true;
	MeshComponent->SetCollisionProfileName(SHIELD_COLLISION_PRESET_NAME);

	UStaticMeshComponent* staticMeshComp = CastChecked<UStaticMeshComponent>(MeshComponent);

	ConstructorHelpers::FObjectFinder<UStaticMesh> staticMesh(TEXT("StaticMesh'/Game/MVRCFPS_Assets/shield09.shield09'"));
	staticMeshComp->SetStaticMesh(staticMesh.Object);
}

FTransform AShieldActor::GetReflectSpawnTransform() const
{
	checkf(MeshComponent->DoesSocketExist(REFLECT_SOCKET_NAME), TEXT("Socket \"AbilityActor\" is missing in the shield actor mesh"));
	return MeshComponent->GetSocketTransform(REFLECT_SOCKET_NAME);
}

void AShieldActor::OnHitInteractable(const ABaseProjectile* projectile)
{
	FTransform socketTransform = GetReflectSpawnTransform();
	projectile->FireProjectile(socketTransform.GetLocation(), socketTransform.Rotator().Vector(), this, Instigator->GetController());

	if (AMlgPlayerController* playerController = GetMlgPlayerController())
	{
		playerController->ClientPlayForceFeedback(playerController->GetRumbleShortLeft(), false, FName("rumbleLeft"));
	}
}