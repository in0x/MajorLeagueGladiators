// Fill out your copyright notice in the Description page of Project Settings.

#include "MajorLeagueGladiator.h"
#include "DpsGrenadeProjectile.h"
#include "ShieldActor.h"
#include "MlgGameplayStatics.h"

ADpsGrenadeProjectile::ADpsGrenadeProjectile(const FObjectInitializer& ObjectInitializer)
	: projectileMovementComponent(ObjectInitializer.CreateDefaultSubobject<UProjectileMovementComponent>(this, TEXT("ProjectileMovementComponent")))
	, explosionMaxDamageRadius(20.f)
	, explosionRadius(200.f)
	, maxDamage(50.f)
	, minDamage(10.f)
	, damageFalloff(0.5f)
	, explosionMaxKnockBack(100.f)
	, timeToExplode(5.f)
{
	bReplicates = true;
	bReplicateMovement = true;
	bStaticMeshReplicateMovement = true;
	GetStaticMeshComponent()->Mobility = EComponentMobility::Movable;
	GetStaticMeshComponent()->SetCollisionObjectType(ECC_Pawn);

	projectileMovementComponent->InitialSpeed = 5000.f;
	projectileMovementComponent->bShouldBounce = true;
	projectileMovementComponent->ProjectileGravityScale = 0.5f;
	projectileMovementComponent->Bounciness = 0.1f;
	
	projectileMovementComponent->OnProjectileBounce.AddDynamic(this, &ADpsGrenadeProjectile::OnProjectileBounce);
	projectileMovementComponent->OnProjectileStop.AddDynamic(this, &ADpsGrenadeProjectile::OnProjectileStop);	
}

void ADpsGrenadeProjectile::FireProjectile(FVector Location, FVector DirectionVector, AActor* ProjectileOwner, AController* ProjectileInstigator) const
{
	FTransform projectileTransform(DirectionVector.ToOrientationRotator(), Location);
	auto* spawnedActor = ProjectileOwner->GetWorld()->SpawnActorDeferred<ADpsGrenadeProjectile>(GetClass(), projectileTransform, ProjectileOwner, ProjectileInstigator->GetPawn());
	auto* spawnedRootComponent = CastChecked<UPrimitiveComponent>(spawnedActor->GetRootComponent());

	spawnedActor->FinishSpawning(projectileTransform);
}

void ADpsGrenadeProjectile::OnProjectileBounce(const FHitResult& ImpactResult, const FVector& ImpactVelocity)
{
	// This event is called by HandleImpact, and checks the velocity after calling it.
	// So by setting the velocity to below the stop threshould, we can stop the projectile.
	if (ImpactResult.Actor->GetRootComponent()->Mobility.GetValue() == EComponentMobility::Static)
	{
		projectileMovementComponent->Velocity = FVector::ZeroVector;
	}	
}

void ADpsGrenadeProjectile::OnProjectileStop(const FHitResult& ImpactResult)
{
	if (AShieldActor* shield = Cast<AShieldActor>(ImpactResult.Actor.Get()))
	{
		Explode();
	}
	else 
	{
		FTimerManager& timer = GetWorldTimerManager();
		timer.SetTimer(explodeTimer, this, &ADpsGrenadeProjectile::Explode, timeToExplode, false);
	}
}

void ADpsGrenadeProjectile::Explode()
{
	UMlgGameplayStatics::ApplyRadialDamageWithFalloff(
		GetWorld(),
		maxDamage,
		minDamage,
		GetActorLocation(),
		explosionMaxDamageRadius,
		explosionRadius,
		damageFalloff,
		DamageType,
		this,
		Instigator->GetController());
	
	Destroy();
}




