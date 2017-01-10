// Fill out your copyright notice in the Description page of Project Settings.

#include "Prototype.h"
#include "GunProjectile.h"

AGunProjectile::AGunProjectile()
{
	PrimaryActorTick.bCanEverTick = true;
}

void AGunProjectile::Tick(float deltaTimeS)
{
	timeAliveS += deltaTimeS;

	if (timeAliveS >= lifeTimeS)
		Destroy();
}


