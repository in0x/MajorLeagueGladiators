// Fill out your copyright notice in the Description page of Project Settings.

#include "MajorLeagueGladiator.h"
#include "MlgGrippableStaticMeshActor.h"
#include "VRControllerComponent.h"

AMlgGrippableStaticMeshActor::AMlgGrippableStaticMeshActor(const FObjectInitializer& ObjectInitializer) {
	bReplicateMovement = true;
}

void AMlgGrippableStaticMeshActor::OnGrip_Implementation(UGripMotionControllerComponent* GrippingController, const FBPActorGripInformation& GripInformation)
{
	UVRControllerComponent* comp = Cast<UVRControllerComponent>(GrippingController);
	currentGrippedController = comp->GetMlgPlayerController();
}

void AMlgGrippableStaticMeshActor::OnGripRelease_Implementation(UGripMotionControllerComponent* ReleasingController, const FBPActorGripInformation& GripInformation)
{
	currentGrippedController = nullptr;
}

AMlgPlayerController* AMlgGrippableStaticMeshActor::GetMlgPlayerController()
{
	return currentGrippedController;
}


