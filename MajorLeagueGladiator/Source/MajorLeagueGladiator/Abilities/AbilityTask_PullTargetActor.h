// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "AbilityTask_PullTargetActor.generated.h"

DECLARE_DELEGATE(FOnLocationReachedDelegate);
DECLARE_DELEGATE(FOnFailDelegate);

UCLASS()
class MAJORLEAGUEGLADIATOR_API AAbilityTask_PullTargetActor : public AActor
{
	GENERATED_BODY()
	
public:	
	AAbilityTask_PullTargetActor();

	virtual void Tick(float DeltaSeconds) override;

	UPROPERTY()
	TWeakObjectPtr<USceneComponent> EndLocationSceneComponent;

	UPROPERTY()
	TWeakObjectPtr<UPrimitiveComponent> TargetRootComponent;

	FOnLocationReachedDelegate OnLocationReached;

	FOnFailDelegate OnFail;

	UPROPERTY()
	float PullSpeed;

	UPROPERTY()
	float MinDistanceThreshold;	

	UPROPERTY()
	float MaxDistanceThreshold;
};
