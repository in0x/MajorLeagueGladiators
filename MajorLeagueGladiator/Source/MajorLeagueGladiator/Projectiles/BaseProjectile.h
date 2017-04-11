// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "BaseProjectile.generated.h"

USTRUCT()
struct FProjectileSpawnParams
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY()
	FVector Scale3D;

	UPROPERTY()
	float DamageScale;

	FProjectileSpawnParams()
		: Scale3D(FVector(1.0f, 1.0f, 1.0f))
		, DamageScale(1.0f)
	{}
};

UCLASS(abstract)
class MAJORLEAGUEGLADIATOR_API ABaseProjectile : public AStaticMeshActor
{
	GENERATED_BODY()

public:	
	ABaseProjectile();

	virtual ABaseProjectile* FireProjectile(FVector Location, FVector DirectionVector, AActor* ProjectileOwner, AController* ProjectileInstigator, 
											const FProjectileSpawnParams& OptionalParams = FProjectileSpawnParams()) const 
	{ checkNoEntry(); return nullptr; }

	UPROPERTY(EditAnywhere)
	float Damage;

	UPROPERTY(EditAnywhere)
	TSubclassOf<UDamageType> DamageType;
};
