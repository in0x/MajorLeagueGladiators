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
	
	if (!closest)
	{
		return false;
	}

	auto gripComp = Cast<IVRGripInterface>(closest->GetRootComponent());
	
	if (gripComp)
	{
		bool foundSlot;
		FTransform slotTrafo;

		gripComp->ClosestPrimarySlotInRange_Implementation(GetComponentLocation(), foundSlot, slotTrafo);

		if (foundSlot)
		{
			GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Green, TEXT("Slot"));
			slotTrafo = UKismetMathLibrary::ConvertTransformToRelative(slotTrafo, closest->GetActorTransform());
			GripActor(closest, slotTrafo, true);
		}
	}
	
	auto gripTrafo = closest->GetTransform();
	gripTrafo.SetLocation(GetComponentLocation());
	GripActor(closest, gripTrafo); 
	
	return true;
}

void UVRControllerComponent::DropAllGrips()
{
	for (auto& grip : GrippedActors)
	{
		DropGrip(grip, true);
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


