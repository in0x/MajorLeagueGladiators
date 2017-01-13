// Fill out your copyright notice in the Description page of Project Settings.

#include "Prototype.h"
#include "GunProjectile.h"

AGunProjectile::AGunProjectile()
{
	PrimaryActorTick.bCanEverTick = true;

	bReplicates = true;
	bReplicateMovement = true;
	bStaticMeshReplicateMovement = true;
}

void AGunProjectile::Tick(float DeltaTimeS)
{
	timeAliveS += DeltaTimeS;
	if (timeAliveS >= lifeTimeS)
		Destroy();
}


