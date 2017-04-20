// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "MlgGrippableActor.h"

#include "MlgGrippableMeshActor.generated.h"

class UVRControllerComponent;
class AMlgPlayerController;

UCLASS()
class MAJORLEAGUEGLADIATOR_API AMlgGrippableMeshActor : public AMlgGrippableActor
{
	GENERATED_BODY()
	
public:	
	static const FName MESH_COMPONENT_NAME;

	AMlgGrippableMeshActor(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	virtual void OnGrip(UGripMotionControllerComponent* GrippingController, const FBPActorGripInformation& GripInformation) override;

	void ReleaseFromGrippedComponent();

	UVRControllerComponent* GetGrippingComponent();

	AMlgPlayerController* GetMlgPlayerController();

	UPROPERTY(EditAnywhere)
	UMeshComponent* MeshComponent;
};
