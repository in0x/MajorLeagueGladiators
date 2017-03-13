// Fill out your copyright notice in the Description page of Project Settings.

#include "MajorLeagueGladiator.h"
#include "MlgGrippableStaticMeshActor.h"
#include "VRControllerComponent.h"

namespace
{
	const char* PACK_COLLISION_PROFILE_NAME = "Pack";
}

AMlgGrippableStaticMeshActor::AMlgGrippableStaticMeshActor(const FObjectInitializer& ObjectInitializer) 
{
	bReplicateMovement = true;
	GetStaticMeshComponent()->SetCollisionProfileName(PACK_COLLISION_PROFILE_NAME);
}

void AMlgGrippableStaticMeshActor::OnGrip(UGripMotionControllerComponent* GrippingController, const FBPActorGripInformation& GripInformation)
{
	//This MUST be called before Super
	ReleaseFromGrippedComponent();

	Super::OnGrip(GrippingController, GripInformation);

	UVRControllerComponent* comp = Cast<UVRControllerComponent>(GrippingController);
	currentGrippedController = comp->GetMlgPlayerController();

	//Instigator should be the one who gripped this actor last. So it can work for sword throwing.
	Instigator = CastChecked<APawn>(GrippingController->GetOwner());
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


