#include "MajorLeagueGladiator.h"
#include "GameplayAbilityTargetActor_Cone.h"

#include "TargetingSplineMeshComponent.h"


AGameplayAbilityTargetActor_Cone::AGameplayAbilityTargetActor_Cone(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
	, Distance(200.0f)
	, halfAngleDegrees(45.0f)
	, halfAngleRadians(FMath::DegreesToRadians(45.0f))
	, IsVisualizingCone(false)
	, AutoConfirm(false)
{
	coneVisualizer = ObjectInitializer.CreateDefaultSubobject<UTargetingSplineMeshComponent>(this, TEXT("SplineMesh"));
}


void AGameplayAbilityTargetActor_Cone::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	TArray<TWeakObjectPtr<AActor>> foundActors = Trace();

	if (foundActors.Num() > 0 && AutoConfirm)
	{
		TargetDataReadyDelegate.Broadcast(MakeTargetHandle(foundActors));
	}
}


void AGameplayAbilityTargetActor_Cone::StartTargeting(UGameplayAbility* Ability)
{
	Super::StartTargeting(Ability);
	SetActorHiddenInGame(IsVisualizingCone);
	if (IsVisualizingCone)
	{
		const float endScale = CalcEndConeScale();
	
		coneVisualizer->SetEndScale({ endScale, endScale });
	}
}


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
	const FVector targetingEnd = targetingLocation + targetingDirecton * Distance;

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

			if (angleDegrees < halfAngleDegrees)
			{
				foundActors.Add(character);
			}
		}
	}

	if (IsVisualizingCone)
	{
		coneVisualizer->SetFromRayCast(targetingLocation, targetingEnd, foundActors.Num() > 0);
	}

	return foundActors;
}

void AGameplayAbilityTargetActor_Cone::SetHalfAngleDegrees(float NewHalfAngleInDegrees)
{
	halfAngleDegrees = NewHalfAngleInDegrees;
	halfAngleRadians = FMath::DegreesToRadians(NewHalfAngleInDegrees);
}


FGameplayAbilityTargetDataHandle AGameplayAbilityTargetActor_Cone::MakeTargetHandle(const TArray<TWeakObjectPtr<AActor>> Actors) const
{
	return StartLocation.MakeTargetDataHandleFromActors(Actors);
}


float AGameplayAbilityTargetActor_Cone::CalcEndConeScale() const
{
	return Distance * FMath::Sin(halfAngleRadians);
}

