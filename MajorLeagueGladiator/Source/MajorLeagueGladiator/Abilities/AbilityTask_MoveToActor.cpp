// Fill out your copyright notice in the Description page of Project Settings.

#include "MajorLeagueGladiator.h"
#include "AbilityTask_MoveToActor.h"


AAbilityTask_MoveToActor::AAbilityTask_MoveToActor()
	: MinDistanceThreshold(10.f)
{
 	PrimaryActorTick.bCanEverTick = true;
}

void AAbilityTask_MoveToActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	const auto location = MovingCharacter->GetActorLocation();
	const auto distance = FVector::Distance(TargetLocation, location); // This needs to happen locally

	if (distance < MinDistanceThreshold /*&& HasAuthority()*/)
	{
		OnLocationReached.Broadcast();
	}
	else /*if (HasAuthority())*/
	{
		const auto direction = (TargetLocation - location).GetSafeNormal() * MoveSpeed;

		MovingCharacter->LaunchCharacter(direction, true, true);
		
		DrawDebugDirectionalArrow(MovingCharacter->GetRootComponent()->GetWorld(), location, TargetLocation, 1.0f, FColor::Green);
	}
}

