// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "IMotionController.h"

#include "HandMotionController.generated.h"

/*
 * Used to be able to use our mock HandComponents with IModularFeature. This way we don't need 
 * to change the GripMotionController, as it's going to use the position from out HandComponents
 * when no actual MotionController are attached.
 */

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class PROTOTYPE_API UHandMotionController : public UObject, public IMotionController
{
	GENERATED_BODY()

public:
	UHandMotionController();
	virtual bool GetControllerOrientationAndPosition(const int32 ControllerIndex, const EControllerHand DeviceHand, FRotator& OutOrientation, FVector& OutPosition) const override;
	virtual ETrackingStatus GetControllerTrackingStatus(const int32 ControllerIndex, const EControllerHand DeviceHand) const override;
};
