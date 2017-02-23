// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "MlgProjectile.h"

#include "GunProjectile.generated.h"

class UProjectileMovementComponent;

UCLASS()
class MAJORLEAGUEGLADIATOR_API AGunProjectile : public AMlgProjectile
{
	GENERATED_BODY()
	
public:
	AGunProjectile(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
	
	virtual void FireProjectile(FVector Location, FVector DirectionVector, AActor* ProjectileOwner, AController* ProjectileInstigator) const override;
	virtual void NotifyActorBeginOverlap(AActor* OtherActor) override;

private:

	void DealDamage(AActor* OtherActor);
	bool IsIgnoredActor(const AActor* Actor) const;

	UPROPERTY()
	UProjectileMovementComponent* projectileMovementComponent;

};
