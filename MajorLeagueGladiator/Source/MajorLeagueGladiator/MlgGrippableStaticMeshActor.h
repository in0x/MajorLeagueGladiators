// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "VRExpansion/GrippableStaticMeshActor.h"
#include "MlgGrippableStaticMeshActor.generated.h"

/**
 * 
 */
class AMlgPlayerController;

UCLASS(Blueprintable, meta = (BlueprintSpawnableComponent))
class MAJORLEAGUEGLADIATOR_API AMlgGrippableStaticMeshActor : public AGrippableStaticMeshActor
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintNativeEvent, Category = "VRGripInterface")
	void OnGrip(UGripMotionControllerComponent * GrippingController, const FBPActorGripInformation & GripInformation);

	UFUNCTION(BlueprintNativeEvent, Category = "VRGripInterface")
	void OnGripRelease(UGripMotionControllerComponent * ReleasingController, const FBPActorGripInformation & GripInformation);

	AMlgPlayerController* getCurrentGrippedController();
private:
	AMlgPlayerController* currentGrippedController;
};
