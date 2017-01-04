// Fill out your copyright notice in the Description page of Project Settings.

#include "Prototype.h"
#include "GunComponent.h"

UGunComponent::UGunComponent()
{
	/*
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.TickInterval = 2.f;

	static ConstructorHelpers::FObjectFinder<UClass> ProjectileClassFinder(TEXT("Blueprint'/Game/Prototype/Content/BluePrints/GunProjectileBP'"));
	GunProjectileClass = ProjectileClassFinder.Object;
	*/
}

void UGunComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UGunComponent::TickComponent( float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction )
{
	/*Super::TickComponent( DeltaTime, TickType, ThisTickFunction );

	if (bFiring)
	{
		GetWorld()->SpawnActor<AGunProjectile>(GunProjectileClass, FActorSpawnParameters());
	}*/
}

