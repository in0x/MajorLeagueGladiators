// Fill out your copyright notice in the Description page of Project Settings.

#include "MajorLeagueGladiator.h"
#include "AbilityTask_PullTargetActor.h"


// Sets default values
AAbilityTask_PullTargetActor::AAbilityTask_PullTargetActor()
{
	PrimaryActorTick.bCanEverTick = true;
}

// Called every frame
void AAbilityTask_PullTargetActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	const FVector targetLocation = TargetRootComponent->GetComponentLocation();
	const FVector endLocation = EndLocationSceneComponent->GetComponentLocation();

	const FVector distanceVec = endLocation - targetLocation;
	const float distance = distanceVec.Size();
	const FVector dirVector = distanceVec / distance;

	const float pullDistance = PullSpeed * DeltaTime;

	//const FVector newLocation = endLocation + dirVector * pullDistance;

	//TargetRootComponent->SetWorldLocation(newLocation);
	TargetRootComponent->MoveComponent(dirVector * pullDistance, TargetRootComponent->GetComponentQuat(), true);

	DrawDebugDirectionalArrow(TargetRootComponent->GetWorld(), targetLocation, endLocation, 1.0f, FColor::Green);

	if (distance - pullDistance < MinDistanceThreshold)
	{
		OnLocationReached.ExecuteIfBound();
	}

}

