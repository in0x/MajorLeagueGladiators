// Fill out your copyright notice in the Description page of Project Settings.

#include "MajorLeagueGladiator.h"
#include "ShieldActor.h"

#include "GunProjectile.h"

#include "MlgProjectile.h"

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
	if (GetStaticMeshComponent()->GetSocketByName(REFLECT_SOCKET_NAME) == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("Socket \"%s\" is missing in the shield actor mesh"), *FString(REFLECT_SOCKET_NAME));
	}
	FTransform socketTransform = GetStaticMeshComponent()->GetSocketTransform(REFLECT_SOCKET_NAME);
	projectile->FireProjectile(socketTransform.GetLocation(), socketTransform.Rotator().Vector(), this, Instigator->GetController());	
}