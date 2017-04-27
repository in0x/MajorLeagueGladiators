// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "BaseProjectile.h"

#include "HitscanProjectile.generated.h"

UCLASS()
class MAJORLEAGUEGLADIATOR_API AHitscanProjectile : public ABaseProjectile
{
	GENERATED_BODY()
	
public:	
	AHitscanProjectile();

	// Returns nullptr as no actual projectile is spawned.
	virtual ABaseProjectile* FireProjectile(FVector Location, FVector DirectionVector, AActor* ProjectileOwner, AController* ProjectileInstigator,
											const FProjectileSpawnParams& OptionalParams = FProjectileSpawnParams()) const override;

	FHitResult Trace(UWorld* world, FVector Location, FVector DirectionVector, const TArray<TWeakObjectPtr<AActor>>& IngnoredActors) const;

private:
	UPROPERTY(EditAnywhere)
	UParticleSystem* beamParticleSystem;

	float range;
};
