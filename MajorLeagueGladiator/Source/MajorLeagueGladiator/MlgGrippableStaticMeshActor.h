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
	AMlgGrippableStaticMeshActor(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	virtual void OnGrip(UGripMotionControllerComponent* GrippingController, const FBPActorGripInformation& GripInformation) override;

	virtual void OnGripRelease(UGripMotionControllerComponent* ReleasingController, const FBPActorGripInformation& GripInformation) override;

	AMlgPlayerController* GetMlgPlayerController();
private:
	AMlgPlayerController* currentGrippedController;
};
