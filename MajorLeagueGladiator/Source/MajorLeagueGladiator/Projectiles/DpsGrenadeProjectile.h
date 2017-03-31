// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Projectiles/BaseProjectile.h"
#include "DpsGrenadeProjectile.generated.h"

UCLASS()
class MAJORLEAGUEGLADIATOR_API ADpsGrenadeProjectile : public ABaseProjectile
{
	GENERATED_BODY()
	
public:
	ADpsGrenadeProjectile(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
	virtual void FireProjectile(FVector Location, FVector DirectionVector, AActor* ProjectileOwner, AController* ProjectileInstigator) const override;
	
private:
	UPROPERTY(EditAnywhere)
	UProjectileMovementComponent* projectileMovementComponent;
	
	UFUNCTION()
	void OnProjectileBounce(const FHitResult& ImpactResult, const FVector& ImpactVelocity);

	UFUNCTION()
	void OnProjectileStop(const FHitResult& ImpactResult);

	void Explode();
};
