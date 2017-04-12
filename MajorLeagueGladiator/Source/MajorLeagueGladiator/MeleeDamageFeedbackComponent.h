// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "DamageFeedbackComponent.h"
#include "MeleeDamageFeedbackComponent.generated.h"

UCLASS(Blueprintable, meta = (BlueprintSpawnableComponent))
class MAJORLEAGUEGLADIATOR_API UMeleeDamageFeedbackComponent : public UDamageFeedbackComponent
{
	GENERATED_BODY()
	
public:

	UMeleeDamageFeedbackComponent();
/*
	UFUNCTION(NetMulticast, reliable)
	virtual void AddVisual_NetMulticast(UMeshComponent* affectedMesh, bool bSpawnParticles, const FPointDamageEvent& PointDamageEvent = FPointDamageEvent(), const UDamageType* DamageType = nullptr) override;
	*/
private:

	
	
};
