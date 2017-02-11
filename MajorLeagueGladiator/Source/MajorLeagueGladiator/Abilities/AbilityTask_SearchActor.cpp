// Fill out your copyright notice in the Description page of Project Settings.

#include "MajorLeagueGladiator.h"
#include "AbilityTask_SearchActor.h"



AAbilityTask_SearchActor::AAbilityTask_SearchActor()
	: maxRange(1000)
{
	PrimaryActorTick.bCanEverTick = true;
}

void AAbilityTask_SearchActor::BeginPlay()
{
	Super::BeginPlay();	
}

void AAbilityTask_SearchActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	const FVector direction = TargetingSceneComponent->GetForwardVector();
	const FVector location = TargetingSceneComponent->GetComponentLocation();
	const FVector end = location + direction * maxRange;

	const TArray<TEnumAsByte<EObjectTypeQuery>> queryTypes{
		UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_WorldStatic),
		UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_WorldDynamic),
		UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_PhysicsBody)
	};

	FHitResult result;
	FCollisionQueryParams CollisionParams;
	CollisionParams.AddIgnoredActors(IgnoredActors);

	UWorld* world = GetWorld();

	world->LineTraceSingleByObjectType(result, location, end, queryTypes, CollisionParams);
	DrawDebugLine(world, location, end, FColor::Green);
	

	if (!result.Actor.IsValid() || !IsValidActor(*result.Actor))
	{
		return;
	}

	TargetDataReadyDelegate.Broadcast(MakeDataHandle(result));

}

bool AAbilityTask_SearchActor::IsConfirmTargetingAllowed()
{
	return false;
}

bool AAbilityTask_SearchActor::IsValidActor(const AActor& Actor) const
{
	return Actor.Implements<UVRGripInterface>();
}

FGameplayAbilityTargetDataHandle AAbilityTask_SearchActor::MakeDataHandle(const FHitResult& HitResult)
{
	return FGameplayAbilityTargetDataHandle(
		new FGameplayAbilityTargetData_SingleTargetHit(HitResult)
		);
}

