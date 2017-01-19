// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Components/ActorComponent.h"
#include "MessageEndpoint.h"
#include "MessageStructs.h"
#include "AmmoComponent.generated.h"

class AGunProjectile;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class PROTOTYPE_API UAmmoComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UAmmoComponent();

	// Returns wether the player still had ammo.
	bool ConsumeAmmo();
	void IncreaseAmmo(uint32 Amount);
	TSubclassOf<AGunProjectile> GetProjectileType();
	
private:
	UPROPERTY(EditAnywhere, Category = "Ammo")
	TSubclassOf<AGunProjectile> gunProjectileClass;
	
	UPROPERTY(EditAnywhere, Category = "Ammo")
	uint32 maxAmmo;
	
	UPROPERTY(EditAnywhere, Category = "Ammo")
	uint32 ammoCount;

	FMessageEndpointPtr msgEndpoint;

	void OnAmmoRefill(const FAmmoRefillMessage& Msg, const IMessageContextRef& Context);
};
