// Fill out your copyright notice in the Description page of Project Settings.

#include "MajorLeagueGladiator.h"
#include "TeleportComponent.h"

UTeleportComponent::UTeleportComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UTeleportComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UTeleportComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	/*
	NOTE(Phil): I'm managing the cooldown because I can't
	manually change the elapsed time on an FTimerHandle.
	*/
	if (elapsedCooldown > 0.f)
	{
		elapsedCooldown -= DeltaTime;

		if (elapsedCooldown < 0.f)
		{
			elapsedCooldown = 0.f;
		}

		OnCooldownChange.Broadcast(elapsedCooldown);
		return;
	}	

	if (!origin)
		return;


	FPredictProjectilePathParams params(0.f, origin->GetComponentLocation(), origin->GetForwardVector() * 750, 2.f);
	params.ObjectTypes = queryTypes;
	params.DrawDebugType = EDrawDebugTrace::ForOneFrame;
	params.DrawDebugTime = 1.f;
	params.bTraceComplex = false;
	params.bTraceWithCollision = true;

	FPredictProjectilePathResult result;

	shouldTeleport = UGameplayStatics::PredictProjectilePath(GetWorld(), params, result);

	tpHitResult = result.HitResult;
	lastTraceDest = result.LastTraceDestination.Location;
	positions.Empty(result.PathData.Num());
	for (const FPredictProjectilePathPointData& PathPoint : result.PathData)
	{
		positions.Add(PathPoint.Location);
	}
}

void UTeleportComponent::Enable(USceneComponent* TeleportOrigin)
{
	origin = TeleportOrigin;
	//Activate();
}

TeleportResult UTeleportComponent::GetTeleportResult()
{
	if (origin)
	{
		if (elapsedCooldown == 0.f)
		{
			elapsedCooldown = cooldown;
		}
		return TeleportResult{ tpHitResult.ImpactPoint, shouldTeleport };
	}
	else
	{
		return TeleportResult{};
	}
}

void UTeleportComponent::Disable()
{
	origin = nullptr;
	//Deactivate();
}

