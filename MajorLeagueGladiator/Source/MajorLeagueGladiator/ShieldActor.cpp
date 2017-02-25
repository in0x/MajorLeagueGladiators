// Fill out your copyright notice in the Description page of Project Settings.

#include "MajorLeagueGladiator.h"
#include "ShieldActor.h"

#include "Projectiles/BaseProjectile.h"
#include "MlgPlayerController.h"

namespace
{
	const char* REFLECT_SOCKET_NAME = "Reflect";
	const char* ABILITY_ACTOR_COLLISION_PRESET_NAME = "AbilityActor";
}

AShieldActor::AShieldActor()
{
	bReplicates = true;
	GetStaticMeshComponent()->bGenerateOverlapEvents = true;
	GetStaticMeshComponent()->SetCollisionProfileName(ABILITY_ACTOR_COLLISION_PRESET_NAME);
}

void AShieldActor::NotifyActorBeginOverlap(AActor* OtherActor)
{
	Super::NotifyActorBeginOverlap(OtherActor);
}

void AShieldActor::OnHitInteractable(const AMlgProjectile* projectile)
{
	checkf(GetStaticMeshComponent()->GetSocketByName(REFLECT_SOCKET_NAME), TEXT("Socket \"AbilityActor\" is missing in the shield actor mesh"));

	FTransform socketTransform = GetStaticMeshComponent()->GetSocketTransform(REFLECT_SOCKET_NAME);
	projectile->FireProjectile(socketTransform.GetLocation(), socketTransform.Rotator().Vector(), this, Instigator->GetController());

	if (AMlgPlayerController* playerController = GetMlgPlayerController())
	{
		playerController->ClientPlayForceFeedback(playerController->GetRumbleShortLeft(), false, FName("rumbleLeft"));
	}
}