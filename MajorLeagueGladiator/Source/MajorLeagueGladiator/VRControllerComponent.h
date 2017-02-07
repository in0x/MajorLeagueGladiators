// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "VRExpansion/GripMotionControllerComponent.h"
#include "VRControllerComponent.generated.h"

struct FMsgDropGrip;

UCLASS(Blueprintable, meta = (BlueprintSpawnableComponent))
class MAJORLEAGUEGLADIATOR_API UVRControllerComponent : public UGripMotionControllerComponent
{
	GENERATED_BODY()

public:
	UVRControllerComponent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
	bool GrabNearestActor(const USphereComponent& GrabSphere);
	void DropAllGrips();
	void DropNonInteractGrips();
	void UseGrippedActors();
	void EndUseGrippedActors();

	bool TryGrabActor(AActor* Actor);

	void SetGripDistance(float NewDistance, int GripIndex = 0);
	float GetGripDistance(int GripIndex = 0) const;
private:

	struct ActorGrabData
	{
		AActor* pActorToGrip;
		IVRGripInterface* pIVRGrip;
	};

	ActorGrabData getNearestGrabableActor(const USphereComponent& GrabSphere) const;
	void GrabActorImpl(ActorGrabData GrabData);
};
