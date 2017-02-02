// Fill out your copyright notice in the Description page of Project Settings.

#include "MajorLeagueGladiator.h"
#include "GunProjectile.h"

AGunProjectile::AGunProjectile()
{
	PrimaryActorTick.bCanEverTick = true;

	bReplicates = true;
	bReplicateMovement = true;
	bStaticMeshReplicateMovement = true;
}

void AGunProjectile::BeginPlay()
{
	Super::BeginPlay();
	OnActorHit.AddDynamic(this, &AGunProjectile::OnHit);
}

void AGunProjectile::OnHit(AActor* SelfActor, AActor* OtherActor, FVector NormalImpulse, const FHitResult& Hit)
{
	Destroy();
}

void AGunProjectile::Tick(float DeltaTimeS)
{
	timeAliveS += DeltaTimeS;
	if (timeAliveS >= lifeTimeS)
		Destroy();
}


