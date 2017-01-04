// Fill out your copyright notice in the Description page of Project Settings.

#include "Prototype.h"
#include "GunActor.h"

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
}

void AGunActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//FTransform trafo;
	//projectileSpawnSocket->GetSocketTransform(trafo, GetStaticMeshComponent());
	//
	//auto projectile = GetWorld()->SpawnActor<AGunProjectile>(GunProjectileClass, trafo);

	//auto projectileSpawnPos = trafo.GetLocation();
	//GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Red, FString::Printf(TEXT("%f, %f, %f"), projectileSpawnPos.X, projectileSpawnPos.Y, projectileSpawnPos.Z));
}




