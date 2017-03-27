// Fill out your copyright notice in the Description page of Project Settings.

#include "MajorLeagueGladiator.h"
#include "WeakpointComponent.h"

UWeakpointComponent::UWeakpointComponent()
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

	for (FWeakpoint& weakpoint : weakpoints)
	{
		auto socket = skeleton->GetSocketByName(weakpoint.LocationSocketName);
		
		if (!socket)
		{
			UE_LOG(DebugLog, Warning, TEXT("Can't find socket for Weakpoint with name %s, removing weakpoint"), *weakpoint.LocationSocketName.ToString());
		}
		else
		{
			weakpoint.MeshSocket = socket;
		}
	}

	std::remove_if(weakpoints.GetData(), weakpoints.GetData() + weakpoints.Num(), [](FWeakpoint& wp)
	{
		return wp.MeshSocket == nullptr;
	});
}

void UWeakpointComponent::TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction)
{
	if (bDebugDrawWeakpoints)
	{
		for (const FWeakpoint& weakpoint : weakpoints)
		{
			DrawDebugSphere(GetWorld(), weakpoint.MeshSocket->GetSocketLocation(skeleton), weakpoint.MaxAffectDistance, 10, FColor::Red);
		}
	}
}

FWeakpoint UWeakpointComponent::FindHitWeakpoint(const FHitResult& Hit) const
{
	const FWeakpoint& closest = *std::min(weakpoints.GetData(), weakpoints.GetData() + weakpoints.Num(), [&](auto& a, auto& b)
	{
		return FVector::DistSquared(a->MeshSocket->GetSocketLocation(skeleton), Hit.Location) 
			 < FVector::DistSquared(b->MeshSocket->GetSocketLocation(skeleton), Hit.Location);
	});

	float affectSqr = closest.MaxAffectDistance * closest.MaxAffectDistance;
	
	if (FVector::DistSquared(closest.MeshSocket->GetSocketLocation(skeleton), Hit.Location) < affectSqr)
	{
		return closest;
	}
	else
	{
		return FWeakpoint();
	}
}


