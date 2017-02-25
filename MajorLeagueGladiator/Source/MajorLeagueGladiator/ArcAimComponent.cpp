// Fill out your copyright notice in the Description page of Project Settings.

#include "MajorLeagueGladiator.h"
#include "ArcAimComponent.h"

UArcAimComponent::UArcAimComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer) 
{
	PrimaryComponentTick.bCanEverTick = true;

	queryTypes = TArray<TEnumAsByte<EObjectTypeQuery>> {
		UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_WorldStatic)
	};
}

void UArcAimComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UArcAimComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
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

	FPredictProjectilePathParams params(0.f, origin->GetComponentLocation(), origin->GetForwardVector() * GetPredictProjectileForce(), 2.f);
	params.ObjectTypes = queryTypes;
	params.DrawDebugType = EDrawDebugTrace::ForOneFrame;
	params.ActorsToIgnore.Add(GetOwner());
	params.DrawDebugTime = 1.f;
	params.bTraceComplex = false;
	params.bTraceWithCollision = true;
	
	FPredictProjectilePathResult result;

	bDidAimFindPosition = UGameplayStatics::PredictProjectilePath(GetWorld(), params, result);

	tpHitResult = result.HitResult;
	lastTraceDest = result.LastTraceDestination.Location;
	positions.Empty(result.PathData.Num());
	for (const FPredictProjectilePathPointData& PathPoint : result.PathData)
	{
		positions.Add(PathPoint.Location);
	}
}

void UArcAimComponent::Enable(USceneComponent* TeleportOrigin)
{
	origin = TeleportOrigin;
}

ArcAimResult UArcAimComponent::GetAimResult()
{
	if (origin)
	{
		if (elapsedCooldown == 0.f)
		{
			elapsedCooldown = cooldown;
		}

		auto result = ArcAimResult{ tpHitResult.ImpactPoint, bDidAimFindPosition };
		bDidAimFindPosition = false;
		return result;
	}
	else
	{
		return ArcAimResult{};
	}
}

void UArcAimComponent::Disable()
{
	origin = nullptr;
}
