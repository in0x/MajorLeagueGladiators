// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Components/ActorComponent.h"
#include "DamageFeedbackComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class MAJORLEAGUEGLADIATOR_API UDamageFeedbackComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UDamageFeedbackComponent();

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION(NetMulticast, reliable)
	virtual void AddVisual_NetMulticast(UMeshComponent* affectedMesh, bool bSpawnParticles, const FTransform& particleTrafo = FTransform(), const UDamageType* DamageType = nullptr) = 0;
		
	
};
