// Fill out your copyright notice in the Description page of Project Settings.

#include "MajorLeagueGladiator.h"
#include "DpsGrenadeProjectile.h"
#include "ShieldActor.h"
#include "MlgGameplayStatics.h"

namespace
{
	const char* PROJECTILE_COLLISION_PROFILE_NAME = "Projectile";
}

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
	TArray<FHitResult> hitResults;
	FindExplodeAffectedActors(hitResults, true);

	for (const FHitResult& hit : hitResults)
	{
		if (UMlgGameplayStatics::CanDealDamageTo(this, hit.Actor.Get()))
		{
			UGameplayStatics::ApplyRadialDamageWithFalloff(
				GetWorld(), 
				maxDamage, 
				minDamage, 
				GetActorLocation(), 
				explosionMaxDamageRadius, 
				explosionRadius,
				damageFalloff,
				DamageType,
				TArray<AActor*>(),
				this,
				Instigator->GetController());
		}
	}

	Destroy();
}

void ADpsGrenadeProjectile::FindExplodeAffectedActors(TArray<FHitResult>& outHits, bool bDrawDebug)
{
	GetWorld()->SweepMultiByChannel(
		outHits,
		GetActorLocation(),
		GetActorLocation(),
		FQuat(),
		ECollisionChannel::ECC_Pawn,
		FCollisionShape::MakeSphere(explosionRadius),
		FCollisionQueryParams()
		);

	outHits.Sort([](const FHitResult& a, const FHitResult& b)
	{
		return a.Actor.Get() < b.Actor.Get();
	});

	for (int32 i = outHits.Num() - 1; i > 0; --i)
	{
		if (outHits[i].Actor == outHits[i - 1].Actor)
		{
			outHits.RemoveAt(i - 1);
		}
	}
	
	if (bDrawDebug)
	{
		UWorld* world = GetWorld();

		DrawDebugSphere(world, GetActorLocation(), explosionRadius, 30, FColor::Magenta, false, 5.0f);

		for (const FHitResult& hit : outHits)
		{
			DrawDebugSphere(world, hit.Actor->GetActorLocation(), 20.0f, 10, FColor::Red, false, 5.f);
			GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Green, FString::Printf(TEXT("%s"), *hit.Actor->GetName()));
		}
	}
}








