// Fill out your copyright notice in the Description page of Project Settings.

#include "Prototype.h"
#include "HandMotionController.h"

UHandMotionController::UHandMotionController()
{
	IModularFeatures::Get().RegisterModularFeature(GetModularFeatureName(), this);
}

bool UHandMotionController::GetControllerOrientationAndPosition(const int32 ControllerIndex, const EControllerHand DeviceHand, FRotator& OutOrientation, FVector& OutPosition) const
{
	return true;
}

ETrackingStatus UHandMotionController::GetControllerTrackingStatus(const int32 ControllerIndex, const EControllerHand DeviceHand) const
{
	return ETrackingStatus::Tracked;
}
