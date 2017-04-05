// Fill out your copyright notice in the Description page of Project Settings.

#include "MajorLeagueGladiator.h"
#include "HitscanProjectile.h"
#include "DamageTypes/PlayerDamage.h"
#include "MlgGameplayStatics.h"
#include "ParticleSystemManagerActor.h"
#include "MlgGameState.h"
#include "ShieldActor.h"
#include "CollisionStatics.h"

AHitscanProjectile::AHitscanProjectile()
	: range(1000000.f)
{
}

ABaseProjectile* AHitscanProjectile::FireProjectile(FVector Location, FVector DirectionVector, AActor* ProjectileOwner, AController* ProjectileInstigator) const
{
	if (Role < ROLE_Authority)
	{
		UE_LOG(DebugLog, Warning, TEXT("Fire Projectile should not be called on client"));
		return nullptr;
	}

	FHitResult hitresult = Trace(ProjectileOwner->GetWorld(), Location, DirectionVector, { ProjectileOwner });
	AActor* hitActor = hitresult.GetActor();

	UGameplayStatics::SpawnEmitterAtLocation(ProjectileOwner->GetWorld(), beamParticleSystem, FTransform(DirectionVector.Rotation().Quaternion(), Location));
	FTransform transf = FTransform(DirectionVector.Rotation().Quaternion(), Location);
	ProjectileOwner->GetWorld()->GetGameState<AMlgGameState>()->GetParticleSystemManager()->CreateParticleSystemMain(beamParticleSystem, transf, true);

	if (hitActor == nullptr)
	{
		return nullptr;
	}

	if (AShieldActor* interactable = Cast<AShieldActor>(hitActor))
	{
		interactable->OnHitInteractable(this);
	}
	else if (UMlgGameplayStatics::CanDealDamageTo(ProjectileInstigator, hitActor))
	{
		UGameplayStatics::ApplyPointDamage(hitActor, damage, -DirectionVector, hitresult, ProjectileInstigator, ProjectileOwner, UPlayerDamage::StaticClass());
	}

	return nullptr;
}

FHitResult AHitscanProjectile::Trace(UWorld* world, FVector Location, FVector DirectionVector, const TArray<TWeakObjectPtr<AActor>>& IngnoredActors) const
{
	FVector end = Location + DirectionVector * range;

	FHitResult result;

	FCollisionQueryParams CollisionParams("HitscanShot", true, Instigator);
	CollisionParams.AddIgnoredActors(IngnoredActors);

	ECollisionChannel hitScanChannel = CollisionStatics::GetCollsionChannelByName(CollisionStatics::HITSCAN_TRACE_CHANNEL_NAME);

	world->LineTraceSingleByChannel(result, Location, end, hitScanChannel, CollisionParams);

	return result;
}