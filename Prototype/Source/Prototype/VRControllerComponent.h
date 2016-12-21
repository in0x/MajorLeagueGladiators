// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "VRExpansion/GripMotionControllerComponent.h"
#include "VRControllerComponent.generated.h"

/**
 * 
 */
UCLASS(Blueprintable, meta = (BlueprintSpawnableComponent))
class PROTOTYPE_API UVRControllerComponent : public UGripMotionControllerComponent
{
	GENERATED_BODY()

public:
	UVRControllerComponent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
	bool GrabNearestActor(const USphereComponent& grabSphere);
	void DropAllGrips();

private:
	AActor* GetNearestGrabableActor(const USphereComponent& grabSphere) const;
};
