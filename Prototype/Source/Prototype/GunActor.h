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
	AGunActor();
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<AGunProjectile> GunProjectileClass;
};
