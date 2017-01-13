// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Engine/StaticMeshActor.h"
#include "GunProjectile.generated.h"

/**
 * 
 */
UCLASS()
class PROTOTYPE_API AGunProjectile : public AStaticMeshActor
{
	GENERATED_BODY()
	
public:
	AGunProjectile();
	virtual void Tick(float DeltaTimeS) override;
	
private:
	// Time until this projectile is destroyed in seconds.
	UPROPERTY(EditAnywhere)
	float lifeTimeS;

	float timeAliveS;
};
