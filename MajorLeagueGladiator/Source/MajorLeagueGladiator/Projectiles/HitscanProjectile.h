// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "BaseProjectile.h"

#include "HitscanProjectile.generated.h"

class AParticleSystemManagerActor;

UCLASS()
class MAJORLEAGUEGLADIATOR_API AHitscanProjectile : public ABaseProjectile
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AHitscanProjectile();

	virtual void FireProjectile(FVector Location, FVector DirectionVector, AActor* ProjectileOwner, AController* ProjectileInstigator) const override;

	FHitResult Trace(UWorld* world, FVector Location, FVector DirectionVector, const TArray<TWeakObjectPtr<AActor>>& IngnoredActors) const;

private:

	UPROPERTY(EditAnywhere)
	UParticleSystem* beamParticleSystem;

	float range;
};
