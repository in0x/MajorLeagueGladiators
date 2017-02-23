// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "MlgProjectile.generated.h"

UCLASS(abstract)
class MAJORLEAGUEGLADIATOR_API AMlgProjectile : public AStaticMeshActor
{
	GENERATED_BODY()

public:	
	AMlgProjectile();

	virtual void FireProjectile(FVector Location, FVector DirectionVector, AActor* ProjectileOwner, AController* ProjectileInstigator) const {	checkNoEntry();	}
	
	UPROPERTY(EditAnywhere)
	float damage;

	UPROPERTY(EditAnywhere)
	TSubclassOf<UDamageType> DamageType;
};
