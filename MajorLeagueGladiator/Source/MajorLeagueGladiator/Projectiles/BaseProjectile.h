// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "BaseProjectile.generated.h"

UCLASS(abstract)
class MAJORLEAGUEGLADIATOR_API ABaseProjectile : public AStaticMeshActor
{
	GENERATED_BODY()

public:	
	ABaseProjectile();

	virtual ABaseProjectile* FireProjectile(FVector Location, FVector DirectionVector, AActor* ProjectileOwner, AController* ProjectileInstigator) const { checkNoEntry(); return nullptr; }

	UPROPERTY(EditAnywhere)
	float damage;

	UPROPERTY(EditAnywhere)
	TSubclassOf<UDamageType> DamageType;
};
