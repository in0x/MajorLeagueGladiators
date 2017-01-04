// Fill out your copyright notice in the Description page of Project Settings.

#include "Prototype.h"
#include "GunActor.h"

AGunActor::AGunActor()
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;
}

void AGunActor::Tick(float DeltaTime)
{
	auto projectile = GetWorld()->SpawnActor<AGunProjectile>(GunProjectileClass, GetActorTransform());

	if (projectile)
	{
		GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Green, TEXT("SPAWNED"));
		projectile->SetActorLocation(GetActorLocation());
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Red, TEXT("FAILED"));
	}
}




