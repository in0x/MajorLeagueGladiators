// Fill out your copyright notice in the Description page of Project Settings.

#include "MajorLeagueGladiator.h"
#include "HitscanProjectile.h"
#include "DamageTypes/HitscanProjectileDamage.h"
#include "MlgGameplayStatics.h"
#include "MlgGameState.h"
#include "ShieldActor.h"
#include "CollisionStatics.h"

AHitscanProjectile::AHitscanProjectile()
	: range(1000000.f)
{
}

ABaseProjectile* AHitscanProjectile::FireProjectile(FVector Location, FVector DirectionVector, AActor* ProjectileOwner, AController* ProjectileInstigator, const FProjectileSpawnParams& OptionalParams) const
{
	if (Role < ROLE_Authority)
	{
		UE_LOG(DebugLog, Warning, TEXT("Fire Projectile should not be called on client"));
		return nullptr;
	}

	FHitResult hitresult = Trace(ProjectileOwner->GetWorld(), Location, DirectionVector, { ProjectileOwner });

	AActor* hitActor = hitresult.GetActor();

	FTransform transform = FTransform(DirectionVector.Rotation().Quaternion(), Location);
	transform.SetScale3D(OptionalParams.Scale3DMultiplier);

	FVector ImpactPoint = hitresult.bBlockingHit ? hitresult.ImpactPoint : Location + DirectionVector * range;

	FBeamEmitterSpawnParams params(beamParticleSystem, transform.GetLocation(), ImpactPoint);
	UMlgGameplayStatics::SpawnBeamEmitterNetworked(ProjectileInstigator->GetWorld(), params);

	if (hitActor == nullptr)
	{
	}
	else if (AShieldActor* interactable = Cast<AShieldActor>(hitActor))
	{
		FTransform reflectTranform = interactable->GetReflectSpawnTransform();
		FireProjectile(reflectTranform.GetLocation(), reflectTranform.GetRotation().GetForwardVector(), ProjectileOwner, ProjectileInstigator, OptionalParams);
	}
	else if (UMlgGameplayStatics::CanDealDamageTo(ProjectileInstigator, hitActor))
	{
		UGameplayStatics::ApplyPointDamage(hitActor, Damage * OptionalParams.DamageScale, -DirectionVector, hitresult, ProjectileInstigator, ProjectileOwner, UHitscanProjectileDamage::StaticClass());
	}

	return nullptr;
}

FHitResult AHitscanProjectile::Trace(UWorld* world, FVector Location, FVector DirectionVector, const TArray<TWeakObjectPtr<AActor>>& IgnoredActors) const
{
	FVector end = Location + DirectionVector * range;

	FHitResult result;

	FCollisionQueryParams CollisionParams("HitscanShot", true, Instigator);
	CollisionParams.AddIgnoredActors(IgnoredActors);

	ECollisionChannel hitScanChannel = CollisionStatics::GetCollsionChannelByName(CollisionStatics::HITSCAN_TRACE_CHANNEL_NAME);

	world->LineTraceSingleByChannel(result, Location, end, hitScanChannel, CollisionParams);

	return result;
}