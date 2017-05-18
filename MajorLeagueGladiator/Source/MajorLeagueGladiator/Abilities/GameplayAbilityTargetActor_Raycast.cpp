#include "MajorLeagueGladiator.h"
#include "GameplayAbilityTargetActor_Raycast.h"
#include "Abilities/GameplayAbility.h"
#include "TargetingSplineMeshComponent.h"
#include "PlayAreaMeshComponent.h"

AGameplayAbilityTargetActor_Raycast::AGameplayAbilityTargetActor_Raycast(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
	, MaxRange(100)
	, EvalTargetFunc([](const FHitResult&) { return true; })
	, CanConfirmInvalidTarget(true)
{
	PrimaryActorTick.bCanEverTick = true;
	splineMesh = ObjectInitializer.CreateDefaultSubobject<UTargetingSplineMeshComponent>(this, TEXT("SplineMesh"));
	playAreaMesh = ObjectInitializer.CreateDefaultSubobject<UPlayAreaMeshComponent>(this, TEXT("PlayAreaMesh"));
}
void AGameplayAbilityTargetActor_Raycast::BeginPlay()
{
	Super::BeginPlay();

	splineMesh->SetStartScale(FVector2D(5.f, 5.f));
	splineMesh->SetEndScale(FVector2D(5.f, 5.f));
}

void AGameplayAbilityTargetActor_Raycast::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	FVector targetLocation;
	FVector direction;
	GetTargetingLocationAndDirection(targetLocation, direction);

	const auto targetBegin = targetLocation;
	const auto targetEnd = targetBegin + (direction * MaxRange);

	const TArray<TEnumAsByte<EObjectTypeQuery>> queryTypes{
		UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_WorldStatic),
	};

	FCollisionQueryParams CollisionParams;
	CollisionParams.AddIgnoredActors(IgnoredActors);

	auto* world = GetWorld();
	auto didHit = world->LineTraceSingleByObjectType(hitResult, targetBegin, targetEnd, queryTypes, CollisionParams);

	bool isValidTarget = EvalTargetFunc(hitResult);

	FVector drawEnd = isValidTarget ? hitResult.ImpactPoint : targetEnd;
	splineMesh->SetFromRayCast(targetBegin, drawEnd, isValidTarget);

	playAreaMesh->SetIsTargetValid(isValidTarget);
	playAreaMesh->SetWorldLocation(splineMesh->GetEndPositionWorld());
}

bool AGameplayAbilityTargetActor_Raycast::IsConfirmTargetingAllowed()
{
	return CanConfirmInvalidTarget || EvalTargetFunc(hitResult);
}

void AGameplayAbilityTargetActor_Raycast::GetTargetingLocationAndDirection(FVector& OutLocation, FVector& OutDirection) const
{
	switch (aimDirection)
	{
	case ERaycastTargetDirection::ForwardVector:
	{
		const FTransform targetTrans = StartLocation.SourceComponent->GetComponentTransform();
		OutDirection = StartLocation.SourceComponent->GetForwardVector();
		OutLocation = targetTrans.GetLocation();
		break;
	}
	case ERaycastTargetDirection::UpVector:
	{
		const FTransform targetTrans = StartLocation.SourceComponent->GetComponentTransform();
		OutDirection = StartLocation.SourceComponent->GetUpVector();
		OutLocation = targetTrans.GetLocation();
		break;
	}
	case ERaycastTargetDirection::ComponentRotation:
	{
		const FTransform targetTrans = StartLocation.GetTargetingTransform();
		OutDirection = targetTrans.GetRotation().Vector();
		OutLocation = targetTrans.GetLocation();
		break;
	}
	}
}

FGameplayAbilityTargetDataHandle AGameplayAbilityTargetActor_Raycast::makeDataHandle()
{
	return FGameplayAbilityTargetDataHandle(
		new FGameplayAbilityTargetData_SingleTargetHit(hitResult)
		);
}

void AGameplayAbilityTargetActor_Raycast::ConfirmTargetingAndContinue()
{
	// I'm explicetly not calling Super since it just broadcast with an empty TargetDataHandle 

	if (IsConfirmTargetingAllowed())
	{
		if (EvalTargetFunc(hitResult))
		{
			TargetDataReadyDelegate.Broadcast(makeDataHandle());
		}
		else
		{
			CancelTargeting();
		}
	}
}

