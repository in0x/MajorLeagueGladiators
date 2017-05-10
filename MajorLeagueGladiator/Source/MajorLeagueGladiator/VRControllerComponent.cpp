// Fill out your copyright notice in the Description page of Project Settings.

#include "MajorLeagueGladiator.h"
#include "VRControllerComponent.h"
#include "MlgPlayerController.h"

#include "CollisionStatics.h"

#include "PackMovementComponent.h"
#include "Characters/MlgPlayerCharacter.h"
#include "BasePack.h"

UVRControllerComponent::UVRControllerComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
	, grabRadius(32)
{}


bool UVRControllerComponent::GrabNearestActor()
{	
	if (GrippedActors.Num() > 0)
		return false;

	auto grabData = getNearestGrabableActor();
	
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

UVRControllerComponent::ActorGrabData UVRControllerComponent::getNearestGrabableActor() const
{
	TArray<FHitResult> hitresults;

	ECollisionChannel gripScanChannel = CollisionStatics::GetCollsionChannelByName(CollisionStatics::GRIPSCAN_TRACE_CHANNEL_NAME);

	GetWorld()->SweepMultiByChannel(hitresults, GetComponentLocation(), GetComponentLocation(),
		FQuat::Identity, gripScanChannel, FCollisionShape::MakeSphere(grabRadius));

	hitresults = hitresults.FilterByPredicate([](const FHitResult& hitresult)
	{
		AActor* actor = hitresult.GetActor();
		// Covers two cases: We have an Actor whose RootComponent is of IVRGripInterface or we have an Actor
		// who is derived from another grippable Actor, such as AGrippableStaticMeshActor.
		return actor && (Cast<IVRGripInterface>(actor->GetRootComponent()) != nullptr || Cast<IVRGripInterface>(actor) != nullptr);
	});

	if (hitresults.Num() == 0)
	{
		return {};
	}

	const FHitResult& closest = std::min(*hitresults.GetData(), hitresults.Last(), [this](const FHitResult& a, const FHitResult& b)
	{
		AActor* actorA = a.GetActor();
		AActor* actorB = b.GetActor();
		return FVector::DistSquared(actorA->GetRootComponent()->GetComponentLocation(), GetComponentLocation())
			 < FVector::DistSquared(actorB->GetRootComponent()->GetComponentLocation(), GetComponentLocation());
	});

	ActorGrabData ret;
	ret.pActorToGrip = closest.GetActor();

	ret.pIVRGrip = Cast<IVRGripInterface>(ret.pActorToGrip->GetRootComponent());
	if (!ret.pIVRGrip)
	{
		ret.pIVRGrip = Cast<IVRGripInterface>(ret.pActorToGrip);
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
	for (auto& grip : LocallyGrippedActors)
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
	for (auto& grip : LocallyGrippedActors)
	{
		auto gripActor = Cast<IVRGripInterface>(CastChecked<AActor>(grip.GrippedObject));

		if (gripActor)
		{
			gripActor->OnEndUsed();
		}
	}
}

bool UVRControllerComponent::HasGrip() const
{
	return GrippedActors.Num() > 0 || LocallyGrippedActors.Num() > 0;
}


AActor* UVRControllerComponent::GetGrippedActor() const
{
	if (GrippedActors.Num() == 0)
	{
		return nullptr;
	}

	AActor* grippedActor = CastChecked<AActor>(GrippedActors[0].GrippedObject);
	return grippedActor;
}

AMlgPlayerController* UVRControllerComponent::GetMlgPlayerController()
{
	AActor* owner = GetOwner();
	APawn* ownerPawn = CastChecked<APawn>(owner);
	AController* controller = ownerPawn->GetController();
	return CastChecked<AMlgPlayerController>(controller,ECastCheckedType::NullAllowed);
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

	if (auto* moveComp = GrabData.pActorToGrip->FindComponentByClass<UPackMovementComponent>())
	{
		moveComp->StopSimulating();
	}

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

bool UVRControllerComponent::LaunchActor(FVector Velocity, bool IgnoreWeight)
{
	if(GrippedActors.Num() == 0)
	{
		UE_LOG(DebugLog, Warning, TEXT("Tried to launch Actor but not actor was present"));
		return false;
	}
	AActor* grippedActor = GetGrippedActor();

	UPrimitiveComponent* rootComp = CastChecked<UPrimitiveComponent>(grippedActor->GetRootComponent());

	//Temporarly disable Collision with this Actor so I don't shoot against myself
	rootComp->MoveIgnoreActors.Add(GetOwner());

	FTimerManager& timer = GetWorld()->GetTimerManager();
	FTimerHandle timerhandle;

	const float TimeUntilClearMoveIgnore = 0.5f;
	timer.SetTimer(timerhandle, rootComp, &UPrimitiveComponent::ClearMoveIgnoreActors, TimeUntilClearMoveIgnore, false);

	auto& grippedActorInfo = GrippedActors[0];

	UPackMovementComponent* moveComp = grippedActorInfo.GetGrippedActor()->FindComponentByClass<UPackMovementComponent>();
	if (moveComp)
	{
		DropGrip(grippedActorInfo, false);
		moveComp->SetVelocity(Velocity);
	}
	else
	{
		DropGrip(grippedActorInfo, true);
		rootComp->AddImpulse(Velocity, NAME_None, IgnoreWeight);
	}
	
	return true;
}