// Fill out your copyright notice in the Description page of Project Settings.

#include "MajorLeagueGladiator.h"
#include "VRControllerComponent.h"
#include "MlgPlayerController.h"

UVRControllerComponent::UVRControllerComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{}


bool UVRControllerComponent::GrabNearestActor(const USphereComponent& GrabSphere)
{	
	if (GrippedActors.Num() > 0)
		return false;

	auto grabData = getNearestGrabableActor(GrabSphere);
	
	if (!grabData.pActorToGrip)
	{
		return false;
	}

	GrabActorImpl(grabData);
	
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
		vrGrip = Cast<IVRGripInterface>(CastChecked<AActor>(GrippedActors[i].GrippedObject));

		if (!vrGrip) 
		{
			vrGrip = Cast<IVRGripInterface>(CastChecked<AActor>(GrippedActors[i].GrippedObject)->GetRootComponent());
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
		auto gripActor = Cast<IVRGripInterface>(CastChecked<AActor>(grip.GrippedObject));

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
		auto gripActor = Cast<IVRGripInterface>(CastChecked<AActor>(grip.GrippedObject));

		if (gripActor)
		{
			gripActor->OnEndUsed();
		}
	}
}

AMlgPlayerController* UVRControllerComponent::GetMlgPlayerController()
{
	return CastChecked<AMlgPlayerController>(CastChecked<APawn>(GetOwner())->Controller);
}


bool UVRControllerComponent::TryGrabActor(AActor* Actor)
{
	if (GrippedActors.Num() > 0)
		return false;

	IVRGripInterface* gripInterface = Cast<IVRGripInterface>(Actor->GetRootComponent());
	if (!gripInterface)
	{
		gripInterface = Cast<IVRGripInterface>(Actor);
		if (gripInterface == nullptr)
		{
			return false;
		}
	}

	GrabActorImpl({ Actor, gripInterface });
	return true;
}

/*
NOTE(Phil)
We now try to find a primary socket in range for gripping first. If we dont find a
socket, we simply attach the Actor as it is at the time of the overlap.
VRExpansion supports gripping sockets, however, the sockets need to be
named either VRGripP (Primary) or VRGripS(Secondary).
IVRGripInterface implementers can be queried using Closest[Primary|Secondary]SlotinRange.
*/
void UVRControllerComponent::GrabActorImpl(ActorGrabData GrabData)
{
	check(GrabData.pActorToGrip);
	check(GrabData.pIVRGrip);
	check(GrippedActors.Num() == 0);

	bool foundSlot;
	FTransform slotTrafo;

	GrabData.pIVRGrip->ClosestPrimarySlotInRange_Implementation(GetComponentLocation(), foundSlot, slotTrafo);
	FTransform actorTransform = GrabData.pActorToGrip->GetTransform();

	if (foundSlot)
	{
		slotTrafo = UKismetMathLibrary::ConvertTransformToRelative(slotTrafo, actorTransform);
		slotTrafo.SetScale3D(actorTransform.GetScale3D());
		GripActor(GrabData.pActorToGrip, slotTrafo, true, FName(), GrabData.pIVRGrip->SlotGripType_Implementation());
	}
	else
	{
		actorTransform.SetLocation(GetComponentLocation());
		GripActor(GrabData.pActorToGrip, actorTransform, false, FName(), GrabData.pIVRGrip->FreeGripType_Implementation());
	}
}

bool UVRControllerComponent::LaunchActor(float Velocity, bool IgnoreWeight)
{
	if(GrippedActors.Num() == 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("Tried to launch Actor but not actor was present"));
		return false;
	}
	AActor* grippedActor = CastChecked<AActor>(GrippedActors[0].GrippedObject);

	UPrimitiveComponent* rootComp = CastChecked<UPrimitiveComponent>(grippedActor->GetRootComponent());

	//Temporarly disable Collision with this Actor so I don't shoot against myself
	rootComp->MoveIgnoreActors.Add(GetOwner());

	FTimerManager& timer = GetWorld()->GetTimerManager();
	FTimerHandle timerhandle;

	const float TimeUntilClearMoveIgnore = 0.5f;
	timer.SetTimer(timerhandle, rootComp, &UPrimitiveComponent::ClearMoveIgnoreActors, TimeUntilClearMoveIgnore, false);

	DropGrip(GrippedActors[0], true);
	rootComp->AddImpulse(GetForwardVector() * Velocity, NAME_None, IgnoreWeight);
	return true;
}