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
	, CollisionChannel(ECollisionChannel::ECC_MAX)
{
	coneVisualizer = ObjectInitializer.CreateDefaultSubobject<UTargetingSplineMeshComponent>(this, TEXT("SplineMesh"));
	RootComponent = coneVisualizer;
	PrimaryActorTick.bCanEverTick = true;
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
	SetActorHiddenInGame(!IsVisualizingCone);
	if (IsVisualizingCone)
	{
		const float endScale = CalcEndConeScale();
	
		coneVisualizer->SetStartScale({ 10.f, 10.f });
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
	FCollisionQueryParams CollisionParams;
	CollisionParams.AddIgnoredActors(IgnoredActors);

	TArray<FOverlapResult> outOverlaps;

	const FTransform targetingTransform = StartLocation.GetTargetingTransform();
	const FVector targetingLocation = targetingTransform.GetLocation();
	const FQuat targetingRotation = targetingTransform.GetRotation();
	const FVector targetingDirecton = targetingRotation.GetForwardVector();
	const FVector targetingEnd = targetingLocation + targetingDirecton * Distance;

	UWorld* world = GetWorld();

	if (QueryTypes.Num() > 0)
	{
		world->OverlapMultiByObjectType(
			outOverlaps,
			targetingLocation,
			targetingRotation,
			FCollisionObjectQueryParams(QueryTypes),
			FCollisionShape::MakeSphere(Distance),
			CollisionParams
			);
	}
	else
	{
		world->OverlapMultiByChannel(
			outOverlaps,
			targetingLocation,
			targetingRotation,
			CollisionChannel,
			FCollisionShape::MakeSphere(Distance),
			CollisionParams
			);
	}
	

	DrawDebugCone(world, targetingLocation, targetingDirecton, Distance, halfAngleRadians, halfAngleRadians, 10, FColor::Emerald);

	TArray<TWeakObjectPtr<AActor>> foundActors;

	for (auto item : outOverlaps)
	{
		auto* actor = item.Actor.Get();
		if (filterFunction && !(filterFunction(actor)))
		{
			continue;
		}
		
		const FVector distance(actor->GetActorLocation() - targetingLocation);
		const FVector normalizedDistance = distance.GetSafeNormal();

		const float angleRadiens = acosf(FVector::DotProduct(normalizedDistance, targetingDirecton));
		const float angleDegrees = FMath::RadiansToDegrees(angleRadiens);

		if (angleDegrees < halfAngleDegrees)
		{
			foundActors.Add(actor);
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
	return Distance * FMath::Sin(halfAngleRadians) * 2;
}

