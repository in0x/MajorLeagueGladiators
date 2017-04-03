// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Projectiles/BaseProjectile.h"
#include "DpsGrenadeProjectile.generated.h"

//TODO(Phil): Shield special move: Spawn 4 new DPSGrenades with smaller radius but same damage, throw in random direction in front of shield
//TODO(Phil): Build a virtual CanBeReflected into baseProjectile, so shield doesnt reflect grenade

UCLASS()
class MAJORLEAGUEGLADIATOR_API ADpsGrenadeProjectile : public ABaseProjectile
{
	GENERATED_BODY()
	
public:
	ADpsGrenadeProjectile(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
	virtual void FireProjectile(FVector Location, FVector DirectionVector, AActor* ProjectileOwner, AController* ProjectileInstigator) const override;
	
private:
	// Radius until which maximum damage will be applied.
	UPROPERTY(EditAnywhere)
	float explosionMaxDamageRadius;

	// Radius that is affected by explosion
	UPROPERTY(EditAnywhere)
	float explosionRadius;

	UPROPERTY(EditAnywhere)
	float maxDamage;

	UPROPERTY(EditAnywhere)
	float minDamage;

	UPROPERTY(EditAnywhere)
	float damageFalloff;

	UPROPERTY(EditAnywhere)
	float explosionMaxKnockBack;

	UPROPERTY(EditAnywhere)
	float timeToExplode;

	UPROPERTY(EditAnywhere)
	UProjectileMovementComponent* projectileMovementComponent;

	FTimerHandle explodeTimer;
	
	UFUNCTION()
	void OnProjectileBounce(const FHitResult& ImpactResult, const FVector& ImpactVelocity);

	UFUNCTION()
	void OnProjectileStop(const FHitResult& ImpactResult);

	void Explode();
};
