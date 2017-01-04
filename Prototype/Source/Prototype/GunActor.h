// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "VRExpansion/GrippableStaticMeshActor.h"
#include "GunProjectile.h"
#include "GunActor.generated.h"

/**
 * 
 */
UCLASS()
class PROTOTYPE_API AGunActor : public AGrippableStaticMeshActor
{
	GENERATED_BODY()
	
public:
	AGunActor(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintNativeEvent, Category = "VRGripInterface")
		void OnGrip(UGripMotionControllerComponent * GrippingController, const FBPActorGripInformation & GripInformation);
	
	UFUNCTION(BlueprintNativeEvent, Category = "VRGripInterface")
		void OnGripRelease(UGripMotionControllerComponent * ReleasingController, const FBPActorGripInformation & GripInformation);

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<AGunProjectile> GunProjectileClass;

	UStaticMeshSocket* projectileSpawnSocket;
};
