// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Components/SphereComponent.h"
#include "ConstraintColliderComponent.generated.h"

/**
 * 
 */
UCLASS(Blueprintable, meta = (BlueprintSpawnableComponent))
class PROTOTYPE_API UConstraintColliderComponent : public USphereComponent
{
	GENERATED_BODY()

private:
	virtual void BeginPlay();

public:
	UConstraintColliderComponent();

	UFUNCTION() void OnBeginOverlap(UPrimitiveComponent* hitComponent, AActor* otherActor, UPrimitiveComponent* otherComp, int32 otherBodyIndex, bool bFromSweep, const FHitResult& hit);
	UFUNCTION() void OnDamageTaken(AActor* DamagedActor, float Damage, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser);


};
