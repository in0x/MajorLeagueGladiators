// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "MlgGrippableStaticMeshActor.h"
#include "ShieldActor.generated.h"


UCLASS()
class MAJORLEAGUEGLADIATOR_API AShieldActor : public AMlgGrippableStaticMeshActor
{
	GENERATED_BODY()
public:
	AShieldActor();

	virtual void NotifyActorBeginOverlap(AActor* OtherActor) override;

	bool IsLocallyControlled() const;
	bool IsReflectableProjectile(AActor* Projectile) const;

	UFUNCTION(Server, Reliable, WithValidation)
	void ReflectProjectile_Server(AActor* Projectile, FVector SocketLocation, FRotator SocketRotation);
	
	
	
};
