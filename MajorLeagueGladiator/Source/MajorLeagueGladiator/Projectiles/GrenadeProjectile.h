// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Projectiles/BaseProjectile.h"
#include "GrenadeProjectile.generated.h"

class AShieldActor;

UCLASS()
class MAJORLEAGUEGLADIATOR_API AGrenadeProjectile : public ABaseProjectile
{
	GENERATED_BODY()
	
public:
	AGrenadeProjectile(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	virtual ABaseProjectile* FireProjectile(FVector Location, FVector DirectionVector, AActor* ProjectileOwner, AController* ProjectileInstigator,
											const FProjectileSpawnParams& OptionalParams = FProjectileSpawnParams()) const override;

	// Radius until which maximum damage will be applied.
	UPROPERTY(EditAnywhere)
	float ExplosionMaxDamageRadius;

	// Radius that is affected by explosion
	UPROPERTY(EditAnywhere)
	float ExplosionRadius;

	UPROPERTY(EditAnywhere)
	float MaxDamage;

	UPROPERTY(EditAnywhere)
	float MinDamage;

	UPROPERTY(EditAnywhere)
	float DamageFalloff;

	UPROPERTY(EditAnywhere)
	float TimeToExplode;

	UPROPERTY(EditAnywhere)
	float InitialSpeed;

	UPROPERTY(EditAnywhere)
	int32 RefractCount;

	UPROPERTY(EditAnywhere)
	float ExplodeSoundMultiplier;

	void TimedExplode();

private:
	UPROPERTY(EditAnywhere)
	UProjectileMovementComponent* projectileMovementComponent;

	UPROPERTY(EditAnywhere)
	USoundCue* explodeSoundCue;

	UPROPERTY(EditAnywhere)
	UParticleSystem* explosionParticles;

	UPROPERTY(EditAnywhere)
	UParticleSystem* smallExplosionParticles;

	UPROPERTY(EditAnywhere)
	UParticleSystemComponent* particleComponent;

	FTimerHandle explodeTimer;
	
	UFUNCTION()
	void onProjectileBounce(const FHitResult& impactResult, const FVector& impactVelocity);

	UFUNCTION()
	void onProjectileStop(const FHitResult& impactResult);

	UFUNCTION()
	void onParticlesFinished(UParticleSystemComponent* System);

	void playExplosionSound();
	void playParticleEffect();
	void refract(AShieldActor* shieldActor);
	void explode();
};
