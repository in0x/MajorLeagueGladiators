// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "AIDamageFeedbackComponent.h"
#include "RangedAIDamageFeedbackComponent.generated.h"

UCLASS(Blueprintable, meta = (BlueprintSpawnableComponent))
class MAJORLEAGUEGLADIATOR_API URangedAIDamageFeedbackComponent : public UAIDamageFeedbackComponent
{
	GENERATED_BODY()
	
public:

	URangedAIDamageFeedbackComponent();

	UFUNCTION(NetMulticast, reliable)
	virtual void DoParticleSystemVisualization_NetMulticast(const FVector& HitLocation, const FVector& OriginDirection, TSubclassOf<UDamageType> DamageType = nullptr) override;

	UFUNCTION(NetMulticast, reliable)
	virtual void DoWeakpointParticleSystemVisualization_NetMulticast(const FVector& HitLocation, const FVector& OriginDirection, TSubclassOf<UDamageType> DamageType = nullptr) override;

private:

	
	
	
};
