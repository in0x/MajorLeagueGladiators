// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Components/ActorComponent.h"
#include "AmmoComponent.generated.h"

class AGunProjectile;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PROTOTYPE_API UAmmoComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UAmmoComponent();
	virtual void BeginPlay() override;
	virtual void TickComponent( float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction ) override;

	// Returns wether the player still had ammo.
	bool PerformShot();
	void IncreaseAmmoCount(uint32 Amount);
	TSubclassOf<AGunProjectile> GetProjectileType();

private:
	UPROPERTY(EditAnywhere, Category = "Ammo")
	TSubclassOf<AGunProjectile> gunProjectileClass;
	
	UPROPERTY(EditAnywhere, Category = "Ammo")
	uint32 maxAmmo;
	
	UPROPERTY(EditAnywhere, Category = "Ammo")
	uint32 ammoCount;
};
