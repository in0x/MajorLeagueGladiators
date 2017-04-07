// Fill out your copyright notice in the Description page of Project Settings.

#include "MajorLeagueGladiator.h"
#include "WeakpointComponent.h"

UWeakpointComponent::UWeakpointComponent()
	: bDebugDrawWeakpoints(false)
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UWeakpointComponent::BeginPlay()
{
	Super::BeginPlay();

	skeleton = GetOwner()->FindComponentByClass<USkeletalMeshComponent>();
	
	if (!skeleton)
	{
		UE_LOG(DebugLog, Warning, TEXT("Parent has no SkeletalMeshComponent, can't match sockets to weakpoints"));
		return;
	}

	weakpoints = weakpoints.FilterByPredicate([&](const FWeakpoint& wp) 
	{
		auto socket = skeleton->GetSocketByName(wp.LocationSocketName);

		if (socket)
		{
			return true;
		}
		else
		{
			UE_LOG(DebugLog, Warning, TEXT("Can't find socket for Weakpoint with name %s, removing weakpoint"), *wp.LocationSocketName.ToString());
			return false;
		}
	});
}

void UWeakpointComponent::TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (bDebugDrawWeakpoints)
	{
		for (const FWeakpoint& weakpoint : weakpoints)
		{
			DrawDebugSphere(GetWorld(), NameToLocation(weakpoint.LocationSocketName), weakpoint.MaxAffectDistance, 10, FColor::Red);
		}
	}
}

FWeakpoint UWeakpointComponent::FindHitWeakpoint(const FHitResult& Hit) const
{
	if (weakpoints.Num() == 0)
	{
		return FWeakpoint();
	}

	FWeakpoint closest = weakpoints[0];
	for (const FWeakpoint& wp : weakpoints)
	{
		if (FVector::DistSquared(NameToLocation(wp.LocationSocketName), Hit.ImpactPoint)
			< FVector::DistSquared(NameToLocation(closest.LocationSocketName), Hit.ImpactPoint))
		{
			closest = wp;
		}
	}

	float affectSqr = closest.MaxAffectDistance * closest.MaxAffectDistance;
	
	if (FVector::DistSquared(NameToLocation(closest.LocationSocketName), Hit.ImpactPoint) < affectSqr)
	{
		GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Emerald,
			FString::Printf(TEXT("Weak point hit, mult: %f"), closest.DamageMultiplier));
		return closest;
	}
	else
	{
		return FWeakpoint();
	}
}

FVector UWeakpointComponent::NameToLocation(const FName& name) const
{
	return skeleton->GetSocketByName(name)->GetSocketLocation(skeleton);
}

