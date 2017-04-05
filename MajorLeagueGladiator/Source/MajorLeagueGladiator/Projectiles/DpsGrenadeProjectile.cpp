// Fill out your copyright notice in the Description page of Project Settings.

#include "MajorLeagueGladiator.h"
#include "DpsGrenadeProjectile.h"
#include "ShieldActor.h"
#include "MlgGameplayStatics.h"

AGrenadeProjectile::AGrenadeProjectile(const FObjectInitializer& ObjectInitializer)
	: projectileMovementComponent(ObjectInitializer.CreateDefaultSubobject<UProjectileMovementComponent>(this, TEXT("ProjectileMovementComponent")))
	, explosionMaxDamageRadius(20.f)
	, explosionRadius(200.f)
	, maxDamage(50.f)
	, minDamage(10.f)
	, damageFalloff(0.5f)
	, timeToExplode(3.f)
	, RefractCount(4)
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
	
	projectileMovementComponent->OnProjectileBounce.AddDynamic(this, &AGrenadeProjectile::OnProjectileBounce);
	projectileMovementComponent->OnProjectileStop.AddDynamic(this, &AGrenadeProjectile::OnProjectileStop);	
}

ABaseProjectile* AGrenadeProjectile::FireProjectile(FVector Location, FVector DirectionVector, AActor* ProjectileOwner, AController* ProjectileInstigator) const
{
	FTransform projectileTransform(DirectionVector.ToOrientationRotator(), Location);
	auto* spawnedActor = ProjectileOwner->GetWorld()->SpawnActorDeferred<AGrenadeProjectile>(GetClass(), projectileTransform, ProjectileOwner, ProjectileInstigator->GetPawn());
	auto* spawnedRootComponent = CastChecked<UPrimitiveComponent>(spawnedActor->GetRootComponent());

	spawnedActor->FinishSpawning(projectileTransform);
	return spawnedActor;
}

void AGrenadeProjectile::OnProjectileBounce(const FHitResult& ImpactResult, const FVector& ImpactVelocity)
{
	// This event is called by HandleImpact, and checks the velocity after calling it.
	// So by setting the velocity to below the stop threshold, we can stop the projectile.
	if (ImpactResult.Actor->GetRootComponent()->Mobility.GetValue() == EComponentMobility::Static ||
		Cast<AShieldActor>(ImpactResult.Actor.Get()))
	{
		projectileMovementComponent->Velocity = FVector::ZeroVector;
	}	

	projectileMovementComponent->ProjectileGravityScale = 1.f;
	projectileMovementComponent->MaxSpeed = 500.f;
}

void AGrenadeProjectile::OnProjectileStop(const FHitResult& ImpactResult)
{
	AActor* impactActor = ImpactResult.Actor.Get();
	
	if (AShieldActor* shield = Cast<AShieldActor>(impactActor))
	{
		Refract(shield);
	}
	else 
	{
		TimedExplode();
	}
}

void AGrenadeProjectile::Refract(AShieldActor* ShieldActor)  
{
	const FTransform spawnTransform = ShieldActor->GetReflectSpawnTransform();
	const FVector sourceLoc = spawnTransform.GetLocation();
	const FVector sourceDir = spawnTransform.GetRotation().GetForwardVector();

	for (int32 i = 0; i < RefractCount; ++i)
	{
		const FRotator rot(FMath::RandRange(-45, 45), FMath::RandRange(-45, 45), FMath::RandRange(-45, 45)); 
		const FVector dir = rot.RotateVector(sourceDir);

		auto* proj = CastChecked<AGrenadeProjectile>(FireProjectile(sourceLoc + sourceDir * 10, dir, GetOwner(), Instigator->GetController()));
		proj->timeToExplode = 1.f;
		proj->explosionRadius = explosionRadius * 0.5f;
		proj->explosionMaxDamageRadius = proj->explosionRadius;

		auto* projMovement = proj->FindComponentByClass<UProjectileMovementComponent>();
		projMovement->ProjectileGravityScale = 1.f;
		projMovement->MaxSpeed = 500.f;

		auto* mesh = proj->FindComponentByClass<UStaticMeshComponent>();
		mesh->IgnoreActorWhenMoving(ShieldActor, true);

		proj->TimedExplode();
	}

	Destroy();
}

void AGrenadeProjectile::Explode()
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
		Instigator->GetController(),
		true);
	
	Destroy();
}

void AGrenadeProjectile::TimedExplode()
{
	FTimerManager& timer = GetWorldTimerManager();
	timer.SetTimer(explodeTimer, this, &AGrenadeProjectile::Explode, timeToExplode, false);

	projectileMovementComponent->OnProjectileBounce.RemoveDynamic(this, &AGrenadeProjectile::OnProjectileBounce);
	projectileMovementComponent->OnProjectileStop.RemoveDynamic(this, &AGrenadeProjectile::OnProjectileStop);
}



