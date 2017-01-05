// Fill out your copyright notice in the Description page of Project Settings.

#include "Prototype.h"
#include "VRControllerComponent.h"
#include <algorithm>

UVRControllerComponent::UVRControllerComponent(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer)
{}

bool UVRControllerComponent::GrabNearestActor(const USphereComponent& grabSphere)
{	
	AActor* closest = GetNearestGrabableActor(grabSphere);

	if (closest)
	{
		GripActor(closest, closest->GetTransform());
		return true;
	}
	
	return false;
}

void UVRControllerComponent::DropAllGrips()
{
	for (int i = GrippedActors.Num()-1; i >= 0; --i)
	{
		DropGrip(GrippedActors[i], true);
	}
}

AActor* UVRControllerComponent::GetNearestGrabableActor(const USphereComponent& grabSphere) const
{
	TArray<AActor*> overlaps;
	grabSphere.GetOverlappingActors(overlaps);

	overlaps = overlaps.FilterByPredicate([](AActor* pActor)
	{
		return Cast<IVRGripInterface>(pActor->GetRootComponent()) != nullptr;
	});

	if (overlaps.Num() == 0)
	{
		return nullptr;
	}
	else if (overlaps.Num() == 1)
		return *overlaps.GetData();

	AActor* closest = std::min(*overlaps.GetData(), overlaps.Last(), [&grabSphere](auto a, auto b)
	{
		return FVector::DistSquared(a->GetRootComponent()->GetComponentLocation(), grabSphere.GetComponentLocation())
			 < FVector::DistSquared(b->GetRootComponent()->GetComponentLocation(), grabSphere.GetComponentLocation());
	});

	return closest;
}


