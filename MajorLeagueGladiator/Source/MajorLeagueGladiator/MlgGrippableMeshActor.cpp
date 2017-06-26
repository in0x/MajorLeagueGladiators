// Fill out your copyright notice in the Description page of Project Settings.

#include "MajorLeagueGladiator.h"
#include "MlgGrippableMeshActor.h"

#include "VRControllerComponent.h"

const FName AMlgGrippableMeshActor::MESH_COMPONENT_NAME(TEXT("Mesh Component"));

AMlgGrippableMeshActor::AMlgGrippableMeshActor(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	MeshComponent = ObjectInitializer.CreateDefaultSubobject<UMeshComponent, UStaticMeshComponent>(this, MESH_COMPONENT_NAME);
	RootComponent = MeshComponent;
}

void AMlgGrippableMeshActor::OnGrip_Implementation(UGripMotionControllerComponent* GrippingController, const FBPActorGripInformation& GripInformation)
{
	Super::OnGrip_Implementation(GrippingController, GripInformation);

	// On Grip Gets called before Set Held is executed so calling this after super is safe
	ReleaseFromGrippedComponent();

	//Instigator should be the one who gripped this actor last. So it can work for sword throwing.
	Instigator = CastChecked<APawn>(GrippingController->GetOwner());
}

void AMlgGrippableMeshActor::ReleaseFromGrippedComponent()
{
	if (VRGripInterfaceSettings.HoldingController)
	{
		VRGripInterfaceSettings.HoldingController->DropActor(this, false);
	}
}

AMlgPlayerController* AMlgGrippableMeshActor::GetMlgPlayerController()
{
	UVRControllerComponent* grippingComponent = GetGrippingComponent();
	return grippingComponent ? grippingComponent->GetMlgPlayerController() : nullptr;
}

UVRControllerComponent* AMlgGrippableMeshActor::GetGrippingComponent()
{
	return CastChecked<UVRControllerComponent>(VRGripInterfaceSettings.HoldingController, ECastCheckedType::NullAllowed);
}


