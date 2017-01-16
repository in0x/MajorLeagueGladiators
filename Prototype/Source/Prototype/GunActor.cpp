// Fill out your copyright notice in the Description page of Project Settings.

#include "Prototype.h"
#include "GunActor.h"
#include "AmmoComponent.h"
#include "GunProjectile.h"

#include "EventBus.h"

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
	UEventBus::Get().CustomEvent.AddDynamic(this, &AGunActor::Test);

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

	if (bShooting && ammoComponent->ConsumeAmmo())
	{
		UEventBus::Get().Fire(&UEventBus::CustomEvent);

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

void AGunActor::Test()
{
	GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Green, TEXT("Event Triggered"));
}




