// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "VRExpansion/GrippableStaticMeshActor.h"
#include "HitScanGunActor.generated.h"

class UGripMotionControllerComponent;
struct FBPActorGripInformation;

UCLASS()
class MAJORLEAGUEGLADIATOR_API AHitScanGunActor : public AGrippableStaticMeshActor
{
	GENERATED_BODY()
	
public:
	AHitScanGunActor(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	virtual void OnUsed() override;
	virtual void OnGrip(UGripMotionControllerComponent * GrippingController, const FBPActorGripInformation & GripInformation) override;
	virtual void OnGripRelease(UGripMotionControllerComponent * ReleasingController, const FBPActorGripInformation & GripInformation) override;

private:

	UPROPERTY(EditAnywhere)
	float shotRange;

	UPROPERTY(EditAnywhere)
	float damage;
	
	UPROPERTY(EditAnywhere)
	UAudioComponent* shotAudioComponent;

	UPROPERTY(EditAnywhere)
	UStaticMeshComponent* laserMesh;

	void shoot();
	
	UStaticMeshSocket* shotOriginSocket;
	UGripMotionControllerComponent* grippingController;

	bool bApplyingRecoil;
};
