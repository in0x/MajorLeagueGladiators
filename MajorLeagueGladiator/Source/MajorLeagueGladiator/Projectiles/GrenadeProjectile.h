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
	virtual ABaseProjectile* FireProjectile(FVector Location, FVector DirectionVector, AActor* ProjectileOwner, AController* ProjectileInstigator) const override;
	
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
	float timeToExplode;

	UPROPERTY(EditAnywhere)
	float InitialSpeed;

	UPROPERTY(EditAnywhere)
	int32 RefractCount;

	void TimedExplode();

private:
	UPROPERTY(EditAnywhere)
	UProjectileMovementComponent* projectileMovementComponent;

	FTimerHandle explodeTimer;
	
	UFUNCTION()
	void OnProjectileBounce(const FHitResult& ImpactResult, const FVector& ImpactVelocity);

	UFUNCTION()
	void OnProjectileStop(const FHitResult& ImpactResult);

	void Refract(AShieldActor* ShieldActor);
	void Explode();
};
