// Fill out your copyright notice in the Description page of Project Settings.

#include "MajorLeagueGladiator.h"
#include "MlgGrippableStaticMeshActor.h"
#include "VRControllerComponent.h"

void AMlgGrippableStaticMeshActor::OnGrip_Implementation(UGripMotionControllerComponent * GrippingController, const FBPActorGripInformation & GripInformation)
{
	UVRControllerComponent* comp = Cast<UVRControllerComponent>(GrippingController);
	currentGrippedController = comp->getMlgPlayerController();
	GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Blue, TEXT("Gripped Lucas"));
}

void AMlgGrippableStaticMeshActor::OnGripRelease_Implementation(UGripMotionControllerComponent * ReleasingController, const FBPActorGripInformation & GripInformation)
{
	currentGrippedController = nullptr;
	GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Blue, TEXT("Releasesd Lucas"));
}

AMlgPlayerController* AMlgGrippableStaticMeshActor::getCurrentGrippedController() 
{
	return currentGrippedController;
}


