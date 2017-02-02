// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Engine/StaticMeshActor.h"
#include "DamageStaticMeshActor.generated.h"

class UHealthComponent;
class UDamageReceiverComponent;

/*
NOTE(Phil): This actor is an example of how to get the HealthComponent to replicate.
*/
UCLASS()
class MAJORLEAGUEGLADIATOR_API ADamageStaticMeshActor : public AStaticMeshActor
{
	GENERATED_BODY()
	
public:
	ADamageStaticMeshActor(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

private:
	UPROPERTY(EditAnywhere, Replicated)
	UHealthComponent* health;

	UPROPERTY(EditAnywhere, Replicated)
	UDamageReceiverComponent* rec;
};
