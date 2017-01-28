// Fill out your copyright notice in the Description page of Project Settings.

#include "Prototype.h"
#include "VRControllerComponent.h"
#include <algorithm>
#include "Messages/MsgDropGrip.h"
#include "MessageEndpointBuilder.h"

UVRControllerComponent::UVRControllerComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{}

void UVRControllerComponent::BeginPlay()
{
	Super::BeginPlay();
	msgEndpoint = FMessageEndpoint::Builder("ControllerMessager").Handling<FMsgDropGrip>(
		this, &UVRControllerComponent::OnDropGripRequest);

	checkf(msgEndpoint.IsValid(), TEXT("UVRControllerComponent Message Endpoint Invalid"));
	msgEndpoint->Subscribe<FMsgDropGrip>();
}

/*
NOTE(Phil)
We now try to find a primary socket in range for gripping first. If we dont find a
socket, we simply attach the Actor as it is at the time of the overlap.
VRExpansion supports gripping sockets, however, the sockets need to be 
named either VRGripP (Primary) or VRGripS(Secondary). 
IVRGripInterface implementers can be queried using Closest[Primary|Secondary]SlotinRange.  
*/
bool UVRControllerComponent::GrabNearestActor(const USphereComponent& GrabSphere)
{	
	auto grabData = getNearestGrabableActor(GrabSphere);
	
	if (!grabData.pActorToGrip)
	{
		return false;
	}

	IVRGripInterface* gripComp = grabData.pIVRGrip;
	AActor* closest = grabData.pActorToGrip;
	FTransform gripTrafo = closest->GetTransform();

	if (gripComp)
	{
		bool foundSlot;
		FTransform slotTrafo;

		gripComp->ClosestPrimarySlotInRange_Implementation(GetComponentLocation(), foundSlot, slotTrafo);

		if (foundSlot)
		{
			slotTrafo = UKismetMathLibrary::ConvertTransformToRelative(slotTrafo, closest->GetActorTransform());
			slotTrafo.SetScale3D(gripTrafo.GetScale3D());
			GripActor(closest, slotTrafo, true, FName(), gripComp->SlotGripType_Implementation());
			return true;
		}
	}
	gripTrafo.SetLocation(GetComponentLocation());
	GripActor(closest, gripTrafo, false, FName(), gripComp->FreeGripType_Implementation());
	
	return true;
}

void UVRControllerComponent::DropAllGrips()
{
	for (int i = GrippedActors.Num()-1; i >= 0; --i)
	{
		DropGrip(GrippedActors[i], true);
	}
}

void UVRControllerComponent::DropNonInteractGrips()
{
	for (int i = GrippedActors.Num() - 1; i >= 0; --i)
	{
		IVRGripInterface* vrGrip;
		vrGrip = Cast<IVRGripInterface>(GrippedActors[i].Actor);

		if (!vrGrip) 
		{
			vrGrip = Cast<IVRGripInterface>(GrippedActors[i].Actor->GetRootComponent());
		}

		if (!vrGrip->IsInteractible_Implementation())
		{
			DropGrip(GrippedActors[i], true);
		}
	}
}

UVRControllerComponent::ActorGrabData UVRControllerComponent::getNearestGrabableActor(const USphereComponent& GrabSphere) const
{
	TArray<AActor*> overlaps;
	GrabSphere.GetOverlappingActors(overlaps);

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

	AActor* closest = std::min(*overlaps.GetData(), overlaps.Last(), [&GrabSphere](auto a, auto b)
	{
		return FVector::DistSquared(a->GetRootComponent()->GetComponentLocation(), GrabSphere.GetComponentLocation())
			 < FVector::DistSquared(b->GetRootComponent()->GetComponentLocation(), GrabSphere.GetComponentLocation());
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

void UVRControllerComponent::UseGrippedActors()
{
	for (auto& grip : GrippedActors)
	{
		auto gripActor = Cast<IVRGripInterface>(grip.Actor);

		if (gripActor)
		{
			gripActor->OnUsed();
		}
	}
}

void UVRControllerComponent::EndUseGrippedActors()
{
	for (auto& grip : GrippedActors)
	{
		auto gripActor = Cast<IVRGripInterface>(grip.Actor);

		if (gripActor)
		{
			gripActor->OnEndUsed();
		}
	}
}

void UVRControllerComponent::OnDropGripRequest(const FMsgDropGrip& Msg, const IMessageContextRef& Context)
{
	DropActor(Msg.ActorToDrop, true);
	/*for (int i = 0; i < GrippedActors.Num(); i++)
	{
		if (GrippedActors[i].Actor == Msg.ActorToDrop)
		{
			handleDropGripRequest_NetMulticast(i);
		}
	}*/
}

void UVRControllerComponent::handleDropGripRequest_NetMulticast_Implementation(int32 GripIndex)
{
	//DropGrip(GrippedActors[GripIndex], true);
}

