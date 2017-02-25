// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "MlgGrippableStaticMeshActor.h"
#include "ShieldActor.generated.h"

class ABaseProjectile;

UCLASS()
class MAJORLEAGUEGLADIATOR_API AShieldActor : public AMlgGrippableStaticMeshActor
{
	GENERATED_BODY()
public:
	AShieldActor();

	virtual void NotifyActorBeginOverlap(AActor* OtherActor) override;

	void OnHitInteractable(const ABaseProjectile* projectile);
	
};
