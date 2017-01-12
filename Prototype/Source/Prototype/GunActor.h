// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "VRExpansion/GrippableStaticMeshActor.h"
#include "GunProjectile.h"
#include "GunActor.generated.h"

/**
 * 
 */
UCLASS()
class PROTOTYPE_API AGunActor : public AGrippableStaticMeshActor
{
	GENERATED_BODY()
	
public:
	AGunActor(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

	virtual void OnUsed() override;	
	virtual void OnEndUsed() override;

private:
	UPROPERTY(EditAnywhere, Category="Gun")
	TSubclassOf<AGunProjectile> gunProjectileClass;
	
	UPROPERTY(EditAnywhere, Category = "Gun")
	float projectileVelAccel = 10000.f;

	// How often, in seconds, this actor shoots.
	UPROPERTY(EditAnywhere, Category = "Gun")
	float shotFrequency = 0.1f;

	UStaticMeshSocket* projectileSpawnSocket;
	
	bool bShooting;
};
