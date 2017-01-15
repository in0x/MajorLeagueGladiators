// Fill out your copyright notice in the Description page of Project Settings.

#include "Prototype.h"
#include "GunActor.h"
#include "AmmoComponent.h"
#include "GunProjectile.h"

AGunActor::AGunActor(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;

	ammoComponent = ObjectInitializer.CreateDefaultSubobject<UAmmoComponent>(this, TEXT("AmmoComponent"));
}

void AGunActor::BeginPlay()
{
	Super::BeginPlay();

	auto mesh = GetStaticMeshComponent()->GetStaticMesh();
	if (mesh)
	{
		projectileSpawnSocket = mesh->FindSocket(FName("ProjectileSpawn"));

		if (!projectileSpawnSocket)
		{
			UE_LOG(DebugLog, Warning, TEXT("Gun Mesh does not have a 'ProjectileSpawn' socket, projectiles will be spawned at incorrect position"));
		}
	}

	SetActorTickInterval(shotFrequency);
}

void AGunActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bShooting && ammoComponent->PerformShot())
	{
		FTransform trafo;
		projectileSpawnSocket->GetSocketTransform(trafo, GetStaticMeshComponent());

		auto projectile = GetWorld()->SpawnActor<AGunProjectile>(ammoComponent->GetProjectileType(), trafo);

		checkf(projectile, TEXT("AGunProjectile spawned by AGunActor was null"));

		projectile->GetStaticMeshComponent()->AddImpulse(GetActorRightVector() * projectileVelAccel);
	}
}

void AGunActor::OnUsed()
{
	bShooting = true;
}

void AGunActor::OnEndUsed()
{
	bShooting = false;
}




