// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "AbilityTask_SearchActor.generated.h"

DECLARE_DELEGATE_OneParam(FOnActorFoundDelegate, AActor*);

UCLASS()
class MAJORLEAGUEGLADIATOR_API AAbilityTask_SearchActor : public AActor
{
	GENERATED_BODY()
	
public:	
	AAbilityTask_SearchActor();

	virtual void BeginPlay() override;
	
	virtual void Tick(float DeltaSeconds) override;

	FOnActorFoundDelegate OnActorFound;

	UPROPERTY()
	TWeakObjectPtr<USceneComponent> TargetingSceneComponent;

	UPROPERTY()
	TArray<TWeakObjectPtr<AActor>> IgnoredActors;

	UPROPERTY()
	float maxRange;

	void SetRelativeTargetingDirection(FVector newRelativeTargetingDirection);

private:
	FVector relativeTargetingDirection;

	bool IsValidActor(const AActor& Actor) const;	
};
