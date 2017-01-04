// Fill out your copyright notice in the Description page of Project Settings.

#include "Prototype.h"
#include "VRControllerComponent.h"
#include <algorithm>

UVRControllerComponent::UVRControllerComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{}

/*
NOTE(Phil)
We now try to find a primary socket in range for gripping first. If we dont find a
socket, we simply attach the Actor as it is at the time of the overlap.
VRExpansion supports gripping sockets, however, the sockets need to be 
named either VRGripP (Primary) or VRGripS(Secondary). 
IVRGripInterface implementers can be queried using Closest[Primary|Secondary]SlotinRange.  
*/
bool UVRControllerComponent::GrabNearestActor(const USphereComponent& grabSphere)
{	
	auto grabData = GetNearestGrabableActor(grabSphere);
	
	if (!grabData.pActorToGrip)
	{
		return false;
	}

	IVRGripInterface* gripComp = grabData.pIVRGrip;
	AActor* closest = grabData.pActorToGrip;
	
	if (gripComp)
	{
		bool foundSlot;
		FTransform slotTrafo;

		gripComp->ClosestPrimarySlotInRange_Implementation(GetComponentLocation(), foundSlot, slotTrafo);

		if (foundSlot)
		{
			slotTrafo = UKismetMathLibrary::ConvertTransformToRelative(slotTrafo, closest->GetActorTransform());
			GripActor(closest, slotTrafo, true);
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, TEXT("From Socket"));
			return true;
		}
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("No IVRGrip"));
	}
	
	auto gripTrafo = closest->GetTransform();
	gripTrafo.SetLocation(GetComponentLocation());
	GripActor(closest, gripTrafo); 
	
	return true;
}

void UVRControllerComponent::DropAllGrips()
{
	if (GrippedActors.Num() == 0)
		return;

	for (auto& grip : GrippedActors)
	{
		DropGrip(grip, true);
	}
}

UVRControllerComponent::ActorGrabData UVRControllerComponent::GetNearestGrabableActor(const USphereComponent& grabSphere) const
{
	TArray<AActor*> overlaps;
	grabSphere.GetOverlappingActors(overlaps);

	overlaps = overlaps.FilterByPredicate([](AActor* pActor)
	{
		// Covers two cases: We have an Actor whose RootComponent is of IVRGripInterface or we have an Actor
		// who is derived from another grippable Actor, such as AGrippableStaticMeshActor.
		return Cast<IVRGripInterface>(pActor->GetRootComponent()) != nullptr || Cast<IVRGripInterface>(pActor) != nullptr;
	});

	if (overlaps.Num() == 0)
	{
		return {};
	}

	AActor* closest = std::min(*overlaps.GetData(), overlaps.Last(), [&grabSphere](auto a, auto b)
	{
		return FVector::DistSquared(a->GetRootComponent()->GetComponentLocation(), grabSphere.GetComponentLocation())
			 < FVector::DistSquared(b->GetRootComponent()->GetComponentLocation(), grabSphere.GetComponentLocation());
	});

	ActorGrabData ret;
	ret.pActorToGrip = closest;

	ret.pIVRGrip = Cast<IVRGripInterface>(closest->GetRootComponent());
	if (!ret.pIVRGrip)
	{
		ret.pIVRGrip = Cast<IVRGripInterface>(closest);
	}
	
	return ret;
}


