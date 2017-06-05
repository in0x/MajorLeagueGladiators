// Fill out your copyright notice in the Description page of Project Settings.

#include "MajorLeagueGladiator.h"
#include "GrenadeProjectile.h"
#include "ShieldActor.h"
#include "MlgGameplayStatics.h"

namespace
{
	const FName GRENADE_COLLISION_PROFILE_NAME = "Grenade";
	const float NORMAL_EXPLOSION_RADIUS = 300.f;
	const float SMALL_EXPLOSION_RADIUS = 150.f;
}

AGrenadeProjectile::AGrenadeProjectile(const FObjectInitializer& ObjectInitializer)
	: projectileMovementComponent(ObjectInitializer.CreateDefaultSubobject<UProjectileMovementComponent>(this, TEXT("ProjectileMovementComponent")))
	, ExplosionMaxDamageRadius(20.f)
	, ExplosionRadius(NORMAL_EXPLOSION_RADIUS)
	, MaxDamage(100.f)
	, MinDamage(20.f)
	, DamageFalloff(0.5f)
	, TimeToExplode(3.f)
	, InitialSpeed(5000.f)
	, RefractCount(4)
	, ExplodeSoundMultiplier(1.0f)
{
	bReplicates = true;
	bReplicateMovement = true;
	bStaticMeshReplicateMovement = true;

	ConstructorHelpers::FObjectFinder<UMaterialInterface> material(TEXT("Material'/Game/Materials/M_Grenade.M_Grenade'"));
	ConstructorHelpers::FObjectFinder<UStaticMesh> mesh(TEXT("StaticMesh'/Engine/BasicShapes/Sphere.Sphere'"));

	ConstructorHelpers::FObjectFinder<USoundCue> explodeSoundCueFinder(TEXT("SoundCue'/Game/MVRCFPS_Assets/Sounds/GrenadeExplosionCue.GrenadeExplosionCue'"));
	explodeSoundCue = explodeSoundCueFinder.Object;

	ConstructorHelpers::FObjectFinder<UParticleSystem> explosionFinder(TEXT("ParticleSystem'/Game/ParticleSystems/GrenadeBlast/GrenadeBlast.GrenadeBlast'"));
	explosionParticles = explosionFinder.Object;

	ConstructorHelpers::FObjectFinder<UParticleSystem> smallExplosionFinder(TEXT("ParticleSystem'/Game/ParticleSystems/GrenadeBlast/GrenadeBlast_small.GrenadeBlast_small'"));
	smallExplosionParticles = smallExplosionFinder.Object;

	UStaticMeshComponent* meshComponent = GetStaticMeshComponent();
	meshComponent->Mobility = EComponentMobility::Movable;
	meshComponent->SetCollisionProfileName(GRENADE_COLLISION_PROFILE_NAME);
	meshComponent->SetWorldScale3D(FVector(0.1f, 0.1f, 0.1f));
	meshComponent->SetStaticMesh(mesh.Object);
	meshComponent->SetMaterial(0, material.Object);

	projectileMovementComponent->InitialSpeed = InitialSpeed;
	projectileMovementComponent->bShouldBounce = true;
	projectileMovementComponent->ProjectileGravityScale = 0.5f;
	projectileMovementComponent->Bounciness = 0.1f;

	projectileMovementComponent->OnProjectileBounce.AddDynamic(this, &AGrenadeProjectile::onProjectileBounce);
	projectileMovementComponent->OnProjectileStop.AddDynamic(this, &AGrenadeProjectile::onProjectileStop);
}

ABaseProjectile* AGrenadeProjectile::FireProjectile(FVector Location, FVector DirectionVector, AActor* ProjectileOwner, AController* ProjectileInstigator, const FProjectileSpawnParams& OptionalParams) const
{
	FTransform projectileTransform(DirectionVector.ToOrientationRotator(), Location);

	APawn* instigator = ProjectileInstigator ? ProjectileInstigator->GetPawn() : nullptr;

	auto* spawnedActor = ProjectileOwner->GetWorld()->SpawnActorDeferred<AGrenadeProjectile>(GetClass(), projectileTransform, ProjectileOwner, instigator);
	spawnedActor->SetActorScale3D(spawnedActor->GetActorScale3D() * OptionalParams.Scale3DMultiplier);
	spawnedActor->Damage *= OptionalParams.DamageScale;

	auto* spawnedRootComponent = CastChecked<UStaticMeshComponent>(spawnedActor->GetRootComponent());
	spawnedRootComponent->IgnoreActorWhenMoving(ProjectileOwner, true);

	spawnedActor->FinishSpawning(projectileTransform);
	return spawnedActor;
}

void AGrenadeProjectile::onProjectileBounce(const FHitResult& ImpactResult, const FVector& ImpactVelocity)
{
	// This event is called by HandleImpact, and checks the velocity after calling it.
	// So by setting the velocity to below the stop threshold, we can stop the projectile.

	AShieldActor* shield = Cast<AShieldActor>(ImpactResult.Actor.Get());
	bool bImpactIsStatic = ImpactResult.Actor->GetRootComponent()->Mobility.GetValue() == EComponentMobility::Static;

	if (shield || bImpactIsStatic)
	{
		projectileMovementComponent->Velocity = FVector::ZeroVector;
	}

	projectileMovementComponent->ProjectileGravityScale = 1.f;
	projectileMovementComponent->MaxSpeed = 500.f;
}

void AGrenadeProjectile::onProjectileStop(const FHitResult& ImpactResult)
{	
	if (AShieldActor* shield = Cast<AShieldActor>(ImpactResult.Actor.Get()))
	{
		refract(shield);
	}
	else
	{
		TimedExplode();
	}
}

void AGrenadeProjectile::refract(AShieldActor* ShieldActor)
{
	const FTransform spawnTransform = ShieldActor->GetReflectSpawnTransform();
	const FVector sourceLocation = spawnTransform.GetLocation();
	const FVector sourceDirection = spawnTransform.GetRotation().GetForwardVector();

	for (int32 i = 0; i < RefractCount; ++i)
	{
		constexpr float angle = 30.f;
		const FRotator randomRotation(FMath::RandRange(-angle, angle), FMath::RandRange(-angle, angle), FMath::RandRange(-angle, angle));
		const FVector launchDirection = randomRotation.RotateVector(sourceDirection);

		auto* spawnedProjectile = CastChecked<AGrenadeProjectile>(FireProjectile(sourceLocation + sourceDirection * 10, launchDirection, GetOwner(), Instigator->GetController()));
		spawnedProjectile->TimeToExplode = 1.f;
		spawnedProjectile->ExplosionRadius = SMALL_EXPLOSION_RADIUS;
		spawnedProjectile->ExplosionMaxDamageRadius = spawnedProjectile->ExplosionRadius;

		auto* spawnedMovement = spawnedProjectile->FindComponentByClass<UProjectileMovementComponent>();
		spawnedMovement->ProjectileGravityScale = 1.f;
		spawnedMovement->MaxSpeed = 500.f;

		auto* spawnedMesh = spawnedProjectile->FindComponentByClass<UStaticMeshComponent>();
		spawnedMesh->IgnoreActorWhenMoving(ShieldActor, true);

		spawnedProjectile->ExplodeSoundMultiplier = 0.25f;
		spawnedProjectile->TimedExplode();
	}

	ShieldActor->PlayReflectEffect();

	Destroy();
}

void AGrenadeProjectile::playExplosionSound()
{
	FSoundParams soundParams(explodeSoundCue, GetActorLocation());
	soundParams.VolumeMultiplier = ExplodeSoundMultiplier;

	UMlgGameplayStatics::PlaySoundNetworked(GetWorld(), soundParams);
}

void AGrenadeProjectile::playParticleEffect()
{
	UParticleSystem* particles = ExplosionRadius == NORMAL_EXPLOSION_RADIUS ? explosionParticles : smallExplosionParticles;

	particleComponent = UGameplayStatics::SpawnEmitterAttached(particles, RootComponent);
	particleComponent->ActivateSystem();
	particleComponent->SetWorldScale3D(FVector(1, 1, 1));

	particleComponent->OnSystemFinished.AddDynamic(this, &AGrenadeProjectile::onParticlesFinished);
}

void AGrenadeProjectile::explode()
{
	UMlgGameplayStatics::ApplyRadialDamageWithFalloff(
		GetWorld(),
		MaxDamage,
		MinDamage,
		GetActorLocation(),
		ExplosionMaxDamageRadius,
		ExplosionRadius,
		DamageFalloff,
		DamageType,
		this,
		Instigator ? Instigator->GetController() : nullptr,
		false);

	playExplosionSound();
	GetStaticMeshComponent()->SetVisibility(false);
}

void AGrenadeProjectile::TimedExplode()
{
	FTimerManager& timerManager = GetWorldTimerManager();
	timerManager.SetTimer(explodeTimer, this, &AGrenadeProjectile::explode, TimeToExplode, false);

	projectileMovementComponent->OnProjectileBounce.RemoveDynamic(this, &AGrenadeProjectile::onProjectileBounce);
	projectileMovementComponent->OnProjectileStop.RemoveDynamic(this, &AGrenadeProjectile::onProjectileStop);

	playParticleEffect();
}

void AGrenadeProjectile::onParticlesFinished(UParticleSystemComponent* System)
{
	Destroy();
}



