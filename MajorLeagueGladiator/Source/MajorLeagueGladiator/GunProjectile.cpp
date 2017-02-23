// Fill out your copyright notice in the Description page of Project Settings.

#include "MajorLeagueGladiator.h"
#include "GunProjectile.h"
#include "MlgPlayerController.h"

#include "ShieldActor.h" //Replace with interface when ready

AGunProjectile::AGunProjectile(const FObjectInitializer& ObjectInitializer)
{
	bReplicates = true;
	bReplicateMovement = true;
	bStaticMeshReplicateMovement = true;
	projectileMovementComponent = ObjectInitializer.CreateDefaultSubobject<UProjectileMovementComponent>(this, "projectileMovementComponent");
}

void AGunProjectile::FireProjectile(FVector Location, FVector DirectionVector, AActor* ProjectileOwner, AController* ProjectileInstigator) const
{
	FTransform projectileTransform(DirectionVector.ToOrientationRotator(), Location);
	AGunProjectile* spawnedActor = GetWorld()->SpawnActorDeferred<AGunProjectile>(GetClass(), projectileTransform, ProjectileOwner, ProjectileInstigator->GetPawn());
	UPrimitiveComponent* spawnedRootComponent = CastChecked<UPrimitiveComponent>(spawnedActor->GetRootComponent());
	spawnedRootComponent->AddImpulse(1000 * DirectionVector, NAME_None, true);

	//Don't collide with shield again
	spawnedRootComponent->MoveIgnoreActors.Add(ProjectileOwner);
	spawnedActor->FinishSpawning(projectileTransform);

}

void AGunProjectile::NotifyActorBeginOverlap(AActor* OtherActor)
{
	Super::NotifyActorBeginOverlap(OtherActor);

	if (Role < ROLE_Authority || IsIgnoredActor(OtherActor))
	{
		return;
	}

	if (AShieldActor* interactable = Cast<AShieldActor>(OtherActor))
	{
		interactable->OnHitInteractable(this);
	}
	else
	{
		DealDamage(OtherActor);
	}
	Destroy();

}

void AGunProjectile::DealDamage(AActor* OtherActor)
{
	FVector travelingDir = GetRootComponent()->GetComponentVelocity().GetSafeNormal();

	UGameplayStatics::ApplyPointDamage(OtherActor, damage, travelingDir, FHitResult{}, Instigator->Controller, this, DamageType);

	if (AMlgPlayerController* mlgController = Cast<AMlgPlayerController>(Instigator))
	{
		mlgController->ClientPlayForceFeedback(mlgController->GetRumbleShortRight(), false, FName("rumbleRight"));
	}
	
}

bool AGunProjectile::IsIgnoredActor(const AActor* Actor) const
{
	return GetOwner() == Actor;
}
