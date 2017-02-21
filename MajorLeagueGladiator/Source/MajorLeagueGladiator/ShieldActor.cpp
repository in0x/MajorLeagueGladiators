// Fill out your copyright notice in the Description page of Project Settings.

#include "MajorLeagueGladiator.h"
#include "ShieldActor.h"

#include "GunProjectile.h"

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
	if (IsLocallyControlled() && IsReflectableProjectile(OtherActor))
	{
		FTransform socketTransform = GetStaticMeshComponent()->GetSocketTransform(REFLECT_SOCKET_NAME);

		ReflectProjectile_Server(OtherActor, socketTransform.GetLocation(), socketTransform.Rotator());
	}

}

bool AShieldActor::IsLocallyControlled() const
{
	AActor* owner = GetOwner();
	APawn* ownerPawn = Cast<APawn>(owner);
	if (ownerPawn)
	{
		return ownerPawn->IsLocallyControlled();
	}

	UE_LOG(LogTemp, Warning, TEXT("Shieldactor was spawned without owner, reflection is disabled now"));
	return false;
}
bool AShieldActor::IsReflectableProjectile(AActor* Projectile) const
{
	return Projectile->IsA<AGunProjectile>();
}

bool AShieldActor::ReflectProjectile_Server_Validate(AActor* Projectile, FVector SocketLocation, FRotator SocketRotation)
{
	return true;
}

void AShieldActor::ReflectProjectile_Server_Implementation(AActor* Projectile, FVector SocketLocation, FRotator SocketRotation)
{
	AActor* spawnedActor = GetWorld()->SpawnActor<AActor>(Projectile->GetClass(), SocketLocation, SocketRotation);
	UPrimitiveComponent* rootComponent = CastChecked<UPrimitiveComponent>(spawnedActor->GetRootComponent());
	rootComponent->AddImpulse(1000 * SocketRotation.Vector(), NAME_None, true);

	Projectile->Destroy();
}