// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "VRExpansion/GripMotionControllerComponent.h"
#include "VRControllerComponent.generated.h"

struct FMsgDropGrip;
class AMlgPlayerController;

UCLASS(Blueprintable, meta = (BlueprintSpawnableComponent))
class MAJORLEAGUEGLADIATOR_API UVRControllerComponent : public UGripMotionControllerComponent
{
	GENERATED_BODY()

public:
	UVRControllerComponent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
	bool GrabNearestActor();
	void DropAllGrips();
	void DropNonInteractGrips();
	void UseGrippedActors();
	void EndUseGrippedActors();
	bool TryGrabActor(AActor* Actor);
	bool LaunchActor(FVector Velocity, bool IgnoreWeight);
	bool HasGrip() const;
	AActor* GetGrippedActor() const;
	AMlgPlayerController* GetMlgPlayerController();

private:

	struct ActorGrabData
	{
		AActor* pActorToGrip;
		IVRGripInterface* pIVRGrip;
	};

	ActorGrabData getNearestGrabableActor() const;
	void GrabActorImpl(ActorGrabData GrabData);

	UPROPERTY(EditAnywhere)
	float grabRadius;
};
