// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "AbilityTask_PullTargetActor.generated.h"

DECLARE_DELEGATE(FOnLocationReachedDelegate);

UCLASS()
class MAJORLEAGUEGLADIATOR_API AAbilityTask_PullTargetActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AAbilityTask_PullTargetActor();

	// Called every frame
	virtual void Tick(float DeltaSeconds) override;

	UPROPERTY()
	TWeakObjectPtr<USceneComponent> EndLocationSceneComponent;

	UPROPERTY()
	TWeakObjectPtr<USceneComponent> TargetRootComponent;

	FOnLocationReachedDelegate OnLocationReached;

	UPROPERTY()
	float PullSpeed;

	UPROPERTY()
	float MinDistanceThreshold;	
};
