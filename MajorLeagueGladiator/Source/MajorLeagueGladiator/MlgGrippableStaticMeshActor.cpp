// Fill out your copyright notice in the Description page of Project Settings.

#include "MajorLeagueGladiator.h"
#include "MlgGrippableStaticMeshActor.h"
#include "VRControllerComponent.h"

AMlgGrippableStaticMeshActor::AMlgGrippableStaticMeshActor(const FObjectInitializer& ObjectInitializer) {
	bReplicateMovement = true;
}

void AMlgGrippableStaticMeshActor::OnGrip(UGripMotionControllerComponent* GrippingController, const FBPActorGripInformation& GripInformation)
{
	Super::OnGrip(GrippingController, GripInformation);
	UVRControllerComponent* comp = Cast<UVRControllerComponent>(GrippingController);
	currentGrippedController = comp->GetMlgPlayerController();
}

void AMlgGrippableStaticMeshActor::OnGripRelease(UGripMotionControllerComponent* ReleasingController, const FBPActorGripInformation& GripInformation)
{
	Super::OnGripRelease(ReleasingController, GripInformation);
	currentGrippedController = nullptr;
}

AMlgPlayerController* AMlgGrippableStaticMeshActor::GetMlgPlayerController()
{
	return currentGrippedController;
}


