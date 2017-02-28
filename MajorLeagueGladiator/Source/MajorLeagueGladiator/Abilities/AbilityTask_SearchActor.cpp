// Fill out your copyright notice in the Description page of Project Settings.

#include "MajorLeagueGladiator.h"
#include "AbilityTask_SearchActor.h"

namespace
{
	const char* OVERLAP_DAMAGE_CAUSER_PROFILE = "OverlapDamageCauser";
}

AAbilityTask_SearchActor::AAbilityTask_SearchActor()
	: MaxRange(1000)
{
	PrimaryActorTick.bCanEverTick = true;
}

void AAbilityTask_SearchActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	const FTransform targetTrans = StartLocation.GetTargetingTransform();

	const FVector direction = targetTrans.GetRotation().Vector();
	
	const FVector rayCastBegin = targetTrans.GetLocation();
	const FVector rayCastEnd = rayCastBegin + direction * MaxRange;

	FCollisionResponseTemplate overlapDamageCauserCollisionTemplate;
	bool success = UCollisionProfile::Get()->GetProfileTemplate(FName(OVERLAP_DAMAGE_CAUSER_PROFILE), overlapDamageCauserCollisionTemplate);
	checkf(success, TEXT("OverlapDamageCauser Collision Profile is missing"));

	const TArray<TEnumAsByte<EObjectTypeQuery>> queryTypes{
		UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_WorldStatic),
		UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_WorldDynamic),
		UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_PhysicsBody),
		UEngineTypes::ConvertToObjectType(overlapDamageCauserCollisionTemplate.ObjectType)
	};

	FHitResult result;
	FCollisionQueryParams CollisionParams;
	CollisionParams.AddIgnoredActors(IgnoredActors);

	UWorld* world = GetWorld();

	world->LineTraceSingleByObjectType(result, rayCastBegin, rayCastEnd, queryTypes, CollisionParams);
	DrawDebugLine(world, rayCastBegin, rayCastEnd, FColor::Green);
	

	if (!result.Actor.IsValid() || !IsPullable(*result.Actor))
	{
		return;
	}

	TargetDataReadyDelegate.Broadcast(MakeDataHandle(result));
}

bool AAbilityTask_SearchActor::IsConfirmTargetingAllowed()
{
	return false;
}

bool AAbilityTask_SearchActor::IsPullable(const AActor& Actor) const
{
	return Actor.Implements<UVRGripInterface>();
}

FGameplayAbilityTargetDataHandle AAbilityTask_SearchActor::MakeDataHandle(const FHitResult& HitResult)
{
	return FGameplayAbilityTargetDataHandle(
		new FGameplayAbilityTargetData_SingleTargetHit(HitResult)
		);
}

