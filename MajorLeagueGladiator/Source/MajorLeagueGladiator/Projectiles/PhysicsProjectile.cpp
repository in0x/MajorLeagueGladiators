// Fill out your copyright notice in the Description page of Project Settings.

#include "MajorLeagueGladiator.h"
#include "PhysicsProjectile.h"
#include "MlgPlayerController.h"

#include "MlgGameplayStatics.h"

#include "ShieldActor.h" //Replace with interface when ready

namespace 
{
	const char* PROJECTILE_COLLISION_PROFILE_NAME = "Projectile";
}

APhysicsProjectile::APhysicsProjectile(const FObjectInitializer& ObjectInitializer)
	: projectileMovementComponent(ObjectInitializer.CreateDefaultSubobject<UProjectileMovementComponent>(this, TEXT("ProjectileMovementComponent")))
{
	bReplicates = true;
	bReplicateMovement = true;
	bStaticMeshReplicateMovement = true;
	GetStaticMeshComponent()->SetCollisionProfileName(PROJECTILE_COLLISION_PROFILE_NAME);
	GetStaticMeshComponent()->Mobility = EComponentMobility::Movable;

	projectileMovementComponent->InitialSpeed = 1000.f;
	projectileMovementComponent->bShouldBounce = false;
	projectileMovementComponent->ProjectileGravityScale = 0;
}

ABaseProjectile* APhysicsProjectile::FireProjectile(FVector Location, FVector DirectionVector, AActor* ProjectileOwner, AController* ProjectileInstigator, const FProjectileSpawnParams& OptionalParams) const
{
	FTransform projectileTransform(DirectionVector.ToOrientationRotator(), Location);
	APhysicsProjectile* spawnedActor = ProjectileOwner->GetWorld()->SpawnActorDeferred<APhysicsProjectile>(GetClass(), projectileTransform, ProjectileOwner, ProjectileInstigator->GetPawn());
	UPrimitiveComponent* spawnedRootComponent = CastChecked<UPrimitiveComponent>(spawnedActor->GetRootComponent());

	//Don't collide with shield again
	spawnedRootComponent->MoveIgnoreActors.Add(ProjectileOwner);
	
	spawnedActor->SetLifeSpan(5.f);
	spawnedActor->projectileMovementComponent->OnProjectileStop.AddDynamic(spawnedActor, &APhysicsProjectile::OnProjectileStop);
	spawnedActor->SetActorScale3D(OptionalParams.Scale3D);
	spawnedActor->Damage *= OptionalParams.DamageScale;

	spawnedActor->FinishSpawning(projectileTransform);
	return spawnedActor;
}

void APhysicsProjectile::NotifyActorBeginOverlap(AActor* OtherActor)
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
	else if (UMlgGameplayStatics::CanDealDamageTo(this, OtherActor))
	{
		DealDamage(OtherActor);
	}	
	//Destroy even if no damage is applied
	Destroy();
}

void APhysicsProjectile::DealDamage(AActor* OtherActor)
{
	FVector travelingDir = GetRootComponent()->GetComponentVelocity().GetSafeNormal();

	UGameplayStatics::ApplyPointDamage(OtherActor, Damage, travelingDir, FHitResult{}, Instigator->Controller, this, DamageType);	
}

bool APhysicsProjectile::IsIgnoredActor(const AActor* Actor) const
{
	return GetOwner() == Actor;
}

void APhysicsProjectile::OnProjectileStop(const FHitResult& ImpactResult)
{
	Destroy();
}
