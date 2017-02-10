// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "VRExpansion/GrippableStaticMeshActor.h"
#include "HitScanGunActor.generated.h"

UCLASS()
class MAJORLEAGUEGLADIATOR_API AHitScanGunActor : public AGrippableStaticMeshActor
{
	GENERATED_BODY()
	
public:
	AHitScanGunActor();
	virtual void BeginPlay() override;
	virtual void OnUsed() override;

private:
	void shoot();

	UPROPERTY(EditAnywhere)
	float shotRange;

	UPROPERTY(EditAnywhere)
	float damage;

	UStaticMeshSocket* shotOriginSocket;
};
