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
	void shoot();

	UPROPERTY(EditAnywhere)
	UAudioComponent* shotAudioComponent;

	UPROPERTY(EditAnywhere)
	UStaticMeshComponent* laserMesh;
	
	UPROPERTY(EditAnywhere)
	float shotRange;

	UPROPERTY(EditAnywhere)
	float damage;

	UPROPERTY(EditAnywhere)
	float recoilAnimBackDuration = 0.5f;

	UPROPERTY(EditAnywhere)
	float recoilAnimForwardDuration = 0.5f;

	float currentAnimDuration;

	float elapsedAnimTime = 0.f;

	float recoilOrigin = 0.f;

	// How far the gun should move back after firing.
	UPROPERTY(EditAnywhere)
	float recoilTarget = -30.f;

	UStaticMeshSocket* shotOriginSocket;
	
	UGripMotionControllerComponent* grippingController;
	
	FBPActorGripInformation gripInfo;

	bool bApplyingRecoil;
};
