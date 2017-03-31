// Fill out your copyright notice in the Description page of Project Settings.

#include "MajorLeagueGladiator.h"
#include "DpsGrenadeProjectile.h"
#include "ShieldActor.h"
#include "CollisionStatics.h"

namespace
{
	const char* PROJECTILE_COLLISION_PROFILE_NAME = "Projectile";
}

ADpsGrenadeProjectile::ADpsGrenadeProjectile(const FObjectInitializer& ObjectInitializer)
	: projectileMovementComponent(ObjectInitializer.CreateDefaultSubobject<UProjectileMovementComponent>(this, TEXT("ProjectileMovementComponent")))
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
	//TODO(Phil): This copied from PhysicsProjectile, change what we dont need

	FTransform projectileTransform(DirectionVector.ToOrientationRotator(), Location);
	ADpsGrenadeProjectile* spawnedActor = ProjectileOwner->GetWorld()->SpawnActorDeferred<ADpsGrenadeProjectile>(GetClass(), projectileTransform, ProjectileOwner, ProjectileInstigator->GetPawn());
	UPrimitiveComponent* spawnedRootComponent = CastChecked<UPrimitiveComponent>(spawnedActor->GetRootComponent());

	spawnedActor->SetLifeSpan(15.f);
	spawnedActor->FinishSpawning(projectileTransform);
}

void ADpsGrenadeProjectile::OnProjectileBounce(const FHitResult& ImpactResult, const FVector& ImpactVelocity)
{
	// This event is called by HandleImpact, and checks the velocity after calling it.
	// So by setting the velocity to below the stop threshould, we can stop the projectile.
	if (ImpactResult.Actor->GetRootComponent()->Mobility.GetValue() == EComponentMobility::Static)
	{
		projectileMovementComponent->Velocity = FVector::ZeroVector;
		GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Orange, TEXT("STOP"));
	}	
}

void ADpsGrenadeProjectile::OnProjectileStop(const FHitResult& ImpactResult)
{
	if (AShieldActor* shield = Cast<AShieldActor>(ImpactResult.Actor.Get()))
	{
		// Instant Explosion
	}
	else 
	{
		// Set off timer and explode
	}
}

void ADpsGrenadeProjectile::Explode()
{
	CollisionStatics::GetCollsionChannelByName(PROJECTILE_COLLISION_PROFILE_NAME);
	//FCollisionQueryParams TraceParams;
	//TraceParams.bTraceComplex = true;
	//TraceParams.bReturnPhysicalMaterial = false;

	////Ignore Actors
	//TraceParams.AddIgnoredActor(ActorToIgnore);

	//
	//TArray<FHitResult> hitResults;
	//ECollisionChannel traceChannel = CollisionStatics::GetCollsionChannelByName(CollisionStatics::GRIPSCAN_TRACE_CHANNEL_NAME);

	//GetWorld()->SweepMultiByChannel(
	//	hitResults,
	//	GetActorLocation(),
	//	GetActorLocation(),
	//	FQuat(),
	//	traceChannel,
	//	FCollisionShape::MakeSphere(Radius),
	//	TraceParams
	//	);
	//
}

void FindExplodeAffectedActors()
{

}






