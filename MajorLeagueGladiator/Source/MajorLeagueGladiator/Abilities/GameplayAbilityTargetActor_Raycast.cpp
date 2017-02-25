#include "MajorLeagueGladiator.h"
#include "GameplayAbilityTargetActor_Raycast.h"
#include "Abilities/GameplayAbility.h"

AGameplayAbilityTargetActor_Raycast::AGameplayAbilityTargetActor_Raycast()
	: MaxRange(100)
	, EvalTargetFunc([](const FHitResult&) { return true; })
{
	PrimaryActorTick.bCanEverTick = true;
}

void AGameplayAbilityTargetActor_Raycast::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	const auto targetTrans = StartLocation.SourceComponent->GetComponentTransform();
	FVector direction;

	switch (aimDirection)
	{
		case ERaycastTargetDirection::ForwardVector:
		{
			direction = StartLocation.SourceComponent->GetForwardVector();
			break;
		}
		case ERaycastTargetDirection::UpVector:
		{
			direction = StartLocation.SourceComponent->GetUpVector();
			break; 
		}
		case ERaycastTargetDirection::ComponentRotation:
		{
			direction = targetTrans.GetRotation().Vector();
			break; 
		}
	}
	
	const auto targetBegin = targetTrans.GetLocation();
	const auto targetEnd = targetBegin + (direction * MaxRange);

	const TArray<TEnumAsByte<EObjectTypeQuery>> queryTypes {
		UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_WorldStatic),
	};

	FHitResult result;
	FCollisionQueryParams CollisionParams;
	CollisionParams.AddIgnoredActors(IgnoredActors);

	auto* world = GetWorld();

	auto didHit = world->LineTraceSingleByObjectType(result, targetBegin, targetEnd, queryTypes, CollisionParams);
	

	if (EvalTargetFunc(result))
	{
		DrawDebugLine(world, targetBegin, targetEnd, FColor::Green);
	}
	else
	{
		DrawDebugLine(world, targetBegin, targetEnd, FColor::Red);
	}

	if (bShouldBroadcastResult)
	{
		if (EvalTargetFunc(result))
			TargetDataReadyDelegate.Broadcast(makeDataHandle(result));
		else
			CancelTargeting();
	}
}

bool AGameplayAbilityTargetActor_Raycast::IsConfirmTargetingAllowed()
{
	return false;
}

FGameplayAbilityTargetDataHandle AGameplayAbilityTargetActor_Raycast::makeDataHandle(const FHitResult& Result) const
{
	return FGameplayAbilityTargetDataHandle(
		new FGameplayAbilityTargetData_SingleTargetHit(Result) 
		);
}
