// Fill out your copyright notice in the Description page of Project Settings.

#include "MajorLeagueGladiator.h"
#include "AbilityTask_PullTargetActor.h"


// Sets default values
AAbilityTask_PullTargetActor::AAbilityTask_PullTargetActor()
	: PullSpeed(500)
	, MinDistanceThreshold(100)
	, MaxDistanceThreshold(2000)
{
	PrimaryActorTick.bCanEverTick = true;
}

// Called every frame
void AAbilityTask_PullTargetActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!TargetRootComponent.IsValid() || TargetRootComponent.IsStale())
	{
		OnFail.ExecuteIfBound();
	}

	const FVector targetLocation = TargetRootComponent->GetComponentLocation();
	const FVector endLocation = EndLocationSceneComponent->GetComponentLocation();

	const FVector distanceVec = endLocation - targetLocation;
	const float distance = distanceVec.Size();

	if (distance < MinDistanceThreshold && Role >= ROLE_Authority)
	{
		OnLocationReached.ExecuteIfBound();
	}
	else if (distance > MaxDistanceThreshold && Role >= ROLE_Authority)
	{
		OnFail.ExecuteIfBound();
	}
	else
	{
		const FVector dirVector = distanceVec / distance;
		TargetRootComponent->SetAllPhysicsLinearVelocity(dirVector * PullSpeed);

		DrawDebugDirectionalArrow(TargetRootComponent->GetWorld(), targetLocation, endLocation, 1.0f, FColor::Green);
	}
}

