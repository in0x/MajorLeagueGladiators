// Fill out your copyright notice in the Description page of Project Settings.

#include "MajorLeagueGladiator.h"
#include "ShootPhysicsProjectileComponent.h"
#include "Projectiles/PhysicsProjectile.h"

UShootPhysicsProjectileComponent::UShootPhysicsProjectileComponent()
	: ProjectileClass(nullptr)
{}

APhysicsProjectile* UShootPhysicsProjectileComponent::ShootInDirection(const FVector& Direction) const
{
	check(GetOwner()->HasAuthority());
	APawn* ownerPawn = CastChecked<APawn>(GetOwner());
	return CastChecked<APhysicsProjectile>(ProjectileClass.GetDefaultObject()->FireProjectile(GetComponentLocation(), Direction, ownerPawn, ownerPawn->GetController()));
}

APhysicsProjectile* UShootPhysicsProjectileComponent::ShootAt(const FVector& TargetLocation) const
{
	return ShootInDirection(TargetLocation - GetComponentLocation());
}

