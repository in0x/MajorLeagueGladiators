// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "VRExpansion/VRGripInterface.h"

#include "MlgGrippableActor.generated.h"

/*
Base Class For Grippable actors. It is just a copy of GripableStaticMeshActor,
but it derives from Actor instead of StaticMeshActor.
*/

UCLASS(Abstract)
class MAJORLEAGUEGLADIATOR_API AMlgGrippableActor : public AActor, public IVRGripInterface
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AMlgGrippableActor(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	//IVRGripInterface
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VRGripInterface")
	FBPInterfaceProperties VRGripInterfaceSettings;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "VRGripInterface")
	bool DenyGripping();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "VRGripInterface")
	EGripInterfaceTeleportBehavior TeleportBehavior();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "VRGripInterface")
	bool SimulateOnDrop();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "VRGripInterface")
	EGripCollisionType SlotGripType();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "VRGripInterface")
	EGripCollisionType FreeGripType();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "VRGripInterface")
	EGripMovementReplicationSettings GripMovementReplicationType();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "VRGripInterface")
	EGripLateUpdateSettings GripLateUpdateSetting();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "VRGripInterface")
	float GripStiffness();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "VRGripInterface")
	float GripDamping();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "VRGripInterface")
	float GripBreakDistance();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "VRGripInterface")
	void ClosestSecondarySlotInRange(FVector WorldLocation, bool & bHadSlotInRange, FTransform & SlotWorldTransform);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "VRGripInterface")
	void ClosestPrimarySlotInRange(FVector WorldLocation, bool & bHadSlotInRange, FTransform & SlotWorldTransform);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "VRGripInterface")
	bool IsInteractible();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "VRGripInterface")
	void IsHeld(UGripMotionControllerComponent *& HoldingController, bool & bIsHeld);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "VRGripInterface")
	void SetHeld(UGripMotionControllerComponent * HoldingController, bool bIsHeld);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "VRGripInterface")
	FBPInteractionSettings GetInteractionSettings();

	void OnEndUsed_Implementation() override { checkNoEntry(); }
	void OnUsed_Implementation() override { checkNoEntry(); }
	void OnEndSecondaryUsed_Implementation() override { checkNoEntry(); }
	void OnSecondaryUsed_Implementation() override { checkNoEntry(); }
	void OnSecondaryGrip_Implementation(USceneComponent* SceneComponent, const FBPActorGripInformation& GripInformation) override { checkNoEntry(); }
	void OnSecondaryGripRelease_Implementation(USceneComponent* SceneComponent, const FBPActorGripInformation& GripInformation) override { checkNoEntry(); }

	void OnChildGrip_Implementation(UGripMotionControllerComponent* GrippingController, const FBPActorGripInformation & GripInformation) override { checkNoEntry(); }
	void OnChildGripRelease_Implementation(UGripMotionControllerComponent* ReleasingController, const FBPActorGripInformation & GripInformation) override { checkNoEntry(); }
	void OnGrip_Implementation(UGripMotionControllerComponent * GrippingController, const FBPActorGripInformation & GripInformation) override { checkNoEntry(); }
	void OnGripRelease_Implementation(UGripMotionControllerComponent * ReleasingController, const FBPActorGripInformation & GripInformation) override { checkNoEntry(); }
	void TickGrip_Implementation(UGripMotionControllerComponent * GrippingController, const FBPActorGripInformation & GripInformation, FVector MControllerLocDelta, float DeltaTime) override {};
	FBPAdvGripPhysicsSettings AdvancedPhysicsSettings_Implementation() override;
	ESecondaryGripType SecondaryGripType_Implementation() override;

};
