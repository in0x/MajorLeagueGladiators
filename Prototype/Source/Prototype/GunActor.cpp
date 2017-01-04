// Fill out your copyright notice in the Description page of Project Settings.

#include "Prototype.h"
#include "GunActor.h"

namespace 
{
	bool bShooting;
}

AGunActor::AGunActor(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;
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

	SetActorTickInterval(0.1f);
}

void AGunActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bShooting)
	{
		FTransform trafo;
		projectileSpawnSocket->GetSocketTransform(trafo, GetStaticMeshComponent());

		auto projectile = GetWorld()->SpawnActor<AGunProjectile>(GunProjectileClass, trafo);
		projectile->GetStaticMeshComponent()->AddImpulse(GetActorRightVector() * 10000.f);
	}
}

void AGunActor::OnGrip_Implementation(UGripMotionControllerComponent * GrippingController, const FBPActorGripInformation & GripInformation)
{
	GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Blue, TEXT("Gripped"));
	bShooting = true;
}

void AGunActor::OnGripRelease_Implementation(UGripMotionControllerComponent * ReleasingController, const FBPActorGripInformation & GripInformation)
{
	GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Blue, TEXT("Grip Release"));
	bShooting = false;
}




