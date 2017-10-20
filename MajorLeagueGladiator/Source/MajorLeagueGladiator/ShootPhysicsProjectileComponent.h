// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "ShootPhysicsProjectileComponent.generated.h"

class APhysicsProjectile;


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class MAJORLEAGUEGLADIATOR_API UShootPhysicsProjectileComponent : public USceneComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UShootPhysicsProjectileComponent();

	APhysicsProjectile* ShootInDirection(const FVector& Direction) const;
	APhysicsProjectile* ShootAt(const FVector& Location) const;

	UPROPERTY(EditAnywhere)
	TSubclassOf<APhysicsProjectile> ProjectileClass;
};
