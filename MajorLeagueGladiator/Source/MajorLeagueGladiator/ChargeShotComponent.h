// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Components/SceneCaptureComponent.h"
#include "ChargeShotComponent.generated.h"

class AHitscanProjectile;

DECLARE_MULTICAST_DELEGATE_OneParam(ChargeValueChangedPercentageDelegate, float);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class MAJORLEAGUEGLADIATOR_API UChargeShotComponent : public USceneComponent
{
	GENERATED_BODY()

public:	
	UChargeShotComponent();
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void Reset();
	float GetValue();
	float GetValuePercentage();

	void FireHitscanShot();

	ChargeValueChangedPercentageDelegate OnChargeValueChangedPercentage;

	UPROPERTY(EditAnywhere)
	float MaxValue;

	UPROPERTY(EditAnywhere)
	float MinValue;

	UPROPERTY(EditAnywhere)
	float TimeToReachMaxValue;

private:
	float currentValue;
	float currentTime;

	UPROPERTY(EditAnywhere)
	TSubclassOf<AHitscanProjectile> projectileClass;
};
