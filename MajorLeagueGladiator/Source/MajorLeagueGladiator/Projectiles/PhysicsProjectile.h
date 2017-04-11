// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "BaseProjectile.h"

#include "PhysicsProjectile.generated.h"

class UProjectileMovementComponent;

UCLASS()
class MAJORLEAGUEGLADIATOR_API APhysicsProjectile : public ABaseProjectile
{
	GENERATED_BODY()
	
public:
	APhysicsProjectile(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
	
	virtual ABaseProjectile* FireProjectile(FVector Location, FVector DirectionVector, AActor* ProjectileOwner, AController* ProjectileInstigator, 
											const FProjectileSpawnParams& OptionalParams = FProjectileSpawnParams()) const override;
	
	virtual void NotifyActorBeginOverlap(AActor* OtherActor) override;

private:
	UPROPERTY(EditAnywhere)
	UProjectileMovementComponent* projectileMovementComponent;

	void DealDamage(AActor* OtherActor);
	bool IsIgnoredActor(const AActor* Actor) const;

	UFUNCTION()
	void OnProjectileStop(const FHitResult& ImpactResult);
};
