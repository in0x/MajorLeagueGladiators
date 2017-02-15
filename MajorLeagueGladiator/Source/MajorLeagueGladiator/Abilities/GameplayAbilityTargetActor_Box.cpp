// Fill out your copyright notice in the Description page of Project Settings.

#include "MajorLeagueGladiator.h"
#include "GameplayAbilityTargetActor_Box.h"

void AGameplayAbilityTargetActor_Box::SetHalfExtent(const FVector& HalfExtent)
{
	boxCollsionShape.SetBox(HalfExtent);
}


void AGameplayAbilityTargetActor_Box::ConfirmTargetingAndContinue()
{
	check(ShouldProduceTargetData());
	if (IsConfirmTargetingAllowed())
	{
		TargetDataReadyDelegate.Broadcast(MakeTargetHandle(Trace()));
	}
}

FVector AGameplayAbilityTargetActor_Box::CalcCenterBox() const
{
	const FTransform trans = StartLocation.GetTargetingTransform();
	const FVector rotatedOffset = trans.GetRotation().RotateVector(LocationOffsetFromPositon);
	return trans.GetLocation() + rotatedOffset;
}

TArray<TWeakObjectPtr<AActor>> AGameplayAbilityTargetActor_Box::Trace() const
{
	const TArray<TEnumAsByte<EObjectTypeQuery>> queryTypes{
		UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_PhysicsBody),
		UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_Pawn)
	};

	FCollisionQueryParams CollisionParams;
	CollisionParams.AddIgnoredActors(IgnoredActors);

	TArray<FOverlapResult> outOverlaps;

	UWorld* world = GetWorld();
	world->OverlapMultiByObjectType(
		outOverlaps,
		CalcCenterBox(),
		StartLocation.GetTargetingTransform().GetRotation(),
		FCollisionObjectQueryParams(queryTypes),
		boxCollsionShape,
		CollisionParams
		);

	DrawDebugBox(world, CalcCenterBox(), boxCollsionShape.GetExtent(), StartLocation.GetTargetingTransform().GetRotation(), FColor::Green, false, 2.0f);

	TArray<TWeakObjectPtr<AActor>> result;
	for (auto item : outOverlaps)
	{
		result.Add(item.Actor);
	}

	return result;
}

FGameplayAbilityTargetDataHandle AGameplayAbilityTargetActor_Box::MakeTargetHandle(const TArray<TWeakObjectPtr<AActor>> Actors) const
{
	return StartLocation.MakeTargetDataHandleFromActors(Actors);
}
