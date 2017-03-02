// Fill out your copyright notice in the Description page of Project Settings.

#include "MajorLeagueGladiator.h"
#include "MlgGrippableStaticMeshActor.h"
#include "VRControllerComponent.h"

AMlgGrippableStaticMeshActor::AMlgGrippableStaticMeshActor(const FObjectInitializer& ObjectInitializer) {
	bReplicateMovement = true;
}

void AMlgGrippableStaticMeshActor::OnGrip(UGripMotionControllerComponent* GrippingController, const FBPActorGripInformation& GripInformation)
{
	//This MUST be called before Super
	ReleaseFromGrippedComponent();

	Super::OnGrip(GrippingController, GripInformation);

	UVRControllerComponent* comp = Cast<UVRControllerComponent>(GrippingController);
	currentGrippedController = comp->GetMlgPlayerController();
}

void AMlgGrippableStaticMeshActor::OnGripRelease(UGripMotionControllerComponent* ReleasingController, const FBPActorGripInformation& GripInformation)
{
	Super::OnGripRelease(ReleasingController, GripInformation);

	currentGrippedController = nullptr;
}

void AMlgGrippableStaticMeshActor::ReleaseFromGrippedComponent()
{
	if (VRGripInterfaceSettings.HoldingController)
	{
		VRGripInterfaceSettings.HoldingController->DropActor(this, false);
	}
}

AMlgPlayerController* AMlgGrippableStaticMeshActor::GetMlgPlayerController()
{
	return currentGrippedController;
}

UVRControllerComponent* AMlgGrippableStaticMeshActor::GetGrippingComponent()
{
	return CastChecked<UVRControllerComponent>(VRGripInterfaceSettings.HoldingController, ECastCheckedType::NullAllowed);
}


