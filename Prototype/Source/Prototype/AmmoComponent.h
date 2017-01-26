// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Components/ActorComponent.h"
#include "MessageEndpoint.h"
#include "AmmoComponent.generated.h"

class AGunProjectile;
struct FMsgAmmoRefill;

DECLARE_MULTICAST_DELEGATE_OneParam(AmmoChangedDelegate, int32);

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class PROTOTYPE_API UAmmoComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UAmmoComponent();
	void BeginPlay();

	// Returns wether the player still had ammo.
	UFUNCTION(NetMulticast, reliable)
	void ConsumeAmmo_NetMulticast();
	
	UFUNCTION(NetMulticast, reliable)
	void IncreaseAmmo_NetMulticast(uint32 Amount);
	
	TSubclassOf<AGunProjectile> GetProjectileType();
	int32 GetAmmoCount() const;
	int32 GetMaxAmmoCount() const;
	
	AmmoChangedDelegate OnAmmoChanged;

private:
	UPROPERTY(EditAnywhere, Category = "Ammo")
	TSubclassOf<AGunProjectile> gunProjectileClass;
	
	UPROPERTY(EditAnywhere, Category = "Ammo")
	uint32 maxAmmo;
	
	UPROPERTY(EditAnywhere, Category = "Ammo")
	uint32 ammoCount;

	FMessageEndpointPtr msgEndpoint;

	void OnAmmoRefill(const FMsgAmmoRefill& Msg, const IMessageContextRef& Context);
};
