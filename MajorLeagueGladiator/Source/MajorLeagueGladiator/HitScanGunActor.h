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
	virtual void OnGrip(UGripMotionControllerComponent* GrippingController, const FBPActorGripInformation& GripInformation) override;
	
private:
	void shoot();

	UFUNCTION(NetMulticast, Reliable)
	void playShotEffect_NetMulticast();

	//UPROPERTY(EditAnywhere)
	//UChildActorComponent* boltAction;

	UPROPERTY(EditAnywhere)
	UAudioComponent* shotAudioComponent;

	UPROPERTY(EditAnywhere)
	UStaticMeshComponent* laserMesh;
	
	UPROPERTY(EditAnywhere)
	float shotRange;

	UPROPERTY(EditAnywhere)
	float damage;

	UPROPERTY(EditAnywhere)
	float recoilAnimBackDuration;

	UPROPERTY(EditAnywhere)
	float recoilAnimForwardDuration;

	float currentAnimDuration;

	float elapsedAnimTime;

	float recoilOrigin;

	// How far the gun should move back after firing.
	UPROPERTY(EditAnywhere)
	float recoilDistance;

	UStaticMeshSocket* shotOriginSocket;
	
	UGripMotionControllerComponent* grippingController;
	
	FBPActorGripInformation gripInfo;

	bool bApplyingRecoil;
};
