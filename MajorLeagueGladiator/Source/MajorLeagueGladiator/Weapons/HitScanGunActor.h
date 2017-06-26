// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Weapons/MlgRangedWeapon.h"
#include "HitScanGunActor.generated.h"

class UGripMotionControllerComponent;
class UAmmoComponent;
class UTextWidget;
class UWidgetComponent;
struct FBPActorGripInformation;
class AHitscanProjectile;
class UChargeShotComponent;
class UPlayerHudWidget;

UCLASS()
class MAJORLEAGUEGLADIATOR_API AHitScanGunActor : public AMlgRangedWeapon
{
	GENERATED_BODY()
	
public:
	AHitScanGunActor(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	virtual void OnUsed_Implementation() override;
	virtual void OnGrip_Implementation(UGripMotionControllerComponent* GrippingController, const FBPActorGripInformation& GripInformation) override;
	
private:
	void shoot();

	UFUNCTION(NetMulticast, Reliable)
	void playShotEffect_NetMulticast(float Charge);

	void playEmptyEffect();

	UPROPERTY(EditAnywhere)
	USceneCaptureComponent2D* sceneCapture;

	UPROPERTY(EditAnywhere)
	UStaticMeshComponent* scopeMesh;

	UPROPERTY(EditAnywhere)
	USoundCue* shotSoundCue;

	UPROPERTY(EditAnywhere)
	USoundCue* shootEmptyCue;

	UPROPERTY(EditAnywhere)
	UStaticMeshComponent* laserMesh;

	UPROPERTY(EditAnywhere)
	UAmmoComponent* ammoComponent;

	UPROPERTY(EditAnywhere)
	UChargeShotComponent* chargeShot;

	UPROPERTY(EditAnywhere)
	UWidgetComponent* ammoCountWidget;

	UPROPERTY(EditAnywhere)
	UWidgetComponent* chargeShotHud;

	UPlayerHudWidget* chargeWidget;

	UPROPERTY(EditAnywhere)
	TSubclassOf<AHitscanProjectile> projectileClass;

	UTextWidget* textWidget;

	UMaterialInstanceDynamic* laserMeshMaterial;

	UMaterialInstanceDynamic* gunMeshMaterial;

	UPROPERTY(EditAnywhere)
	float MaxGlow;

	UPROPERTY(EditAnywhere)
	float MinGlow;

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

	float adjustedRecoilDistance;
	
	UGripMotionControllerComponent* grippingController;
	
	FBPActorGripInformation gripInfo;

	bool bIsApplyingRecoil;
};
