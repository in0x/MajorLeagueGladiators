// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "VRExpansion/GripMotionControllerComponent.h"
#include "MessageEndpoint.h"
#include "VRControllerComponent.generated.h"

struct FMsgDropGrip;

UCLASS(Blueprintable, meta = (BlueprintSpawnableComponent))
class PROTOTYPE_API UVRControllerComponent : public UGripMotionControllerComponent
{
	GENERATED_BODY()

public:
	UVRControllerComponent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
	void BeginPlay();
	bool GrabNearestActor(const USphereComponent& GrabSphere);
	void DropAllGrips();
	void DropManipulationGrips();
	void UseGrippedActors();
	void EndUseGrippedActors();

private:

	struct ActorGrabData
	{
		AActor* pActorToGrip;
		IVRGripInterface* pIVRGrip;
	};

	ActorGrabData getNearestGrabableActor(const USphereComponent& GrabSphere) const;
	void OnDropGripRequest(const FMsgDropGrip& Msg, const IMessageContextRef& Context);

	FMessageEndpointPtr msgEndpoint;
};
