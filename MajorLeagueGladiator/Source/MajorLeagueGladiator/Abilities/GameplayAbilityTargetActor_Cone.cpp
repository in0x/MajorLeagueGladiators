#include "MajorLeagueGladiator.h"
#include "GameplayAbilityTargetActor_Cone.h"


void AGameplayAbilityTargetActor_Cone::ConfirmTargetingAndContinue()
{
	check(ShouldProduceTargetData());
	if (IsConfirmTargetingAllowed())
	{
		TargetDataReadyDelegate.Broadcast(MakeTargetHandle(Trace()));
	}
}

TArray<TWeakObjectPtr<AActor>> AGameplayAbilityTargetActor_Cone::Trace() const
{
	const TArray<TEnumAsByte<EObjectTypeQuery>> queryTypes{
		UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_Pawn)
	};

	FCollisionQueryParams CollisionParams;
	CollisionParams.AddIgnoredActors(IgnoredActors);

	TArray<FOverlapResult> outOverlaps;

	const FTransform targetingTransform = StartLocation.GetTargetingTransform();
	const FVector targetingLocation = targetingTransform.GetLocation();
	const FQuat targetingRotation = targetingTransform.GetRotation();
	const FVector targetingDirecton = targetingRotation.GetForwardVector();

	UWorld* world = GetWorld();
	world->OverlapMultiByObjectType(
		outOverlaps,
		targetingLocation,
		targetingRotation,
		FCollisionObjectQueryParams(queryTypes),
		FCollisionShape::MakeSphere(Distance),
		CollisionParams
		);

	TArray<TWeakObjectPtr<AActor>> foundActors;
	for (auto item : outOverlaps)
	{
		if (ACharacter* character = Cast<ACharacter>(item.Actor.Get()))
		{
			const FVector distance(character->GetActorLocation() - targetingLocation);
			const FVector normalizedDistance = distance.GetSafeNormal();

			const float angleRadiens = acosf(FVector::DotProduct(normalizedDistance, targetingDirecton));
			const float angleDegrees = FMath::RadiansToDegrees(angleRadiens);

			if (angleDegrees < HalfAngleDegrees)
			{
				foundActors.Add(character);
			}
		}
	}

	return foundActors;
}

FGameplayAbilityTargetDataHandle AGameplayAbilityTargetActor_Cone::MakeTargetHandle(const TArray<TWeakObjectPtr<AActor>> Actors) const
{
	return StartLocation.MakeTargetDataHandleFromActors(Actors);
}
