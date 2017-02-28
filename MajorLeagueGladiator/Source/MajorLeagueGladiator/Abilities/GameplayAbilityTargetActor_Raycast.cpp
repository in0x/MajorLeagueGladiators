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

	FCollisionQueryParams CollisionParams;
	CollisionParams.AddIgnoredActors(IgnoredActors);

	auto* world = GetWorld();

	auto didHit = world->LineTraceSingleByObjectType(hitResult, targetBegin, targetEnd, queryTypes, CollisionParams);
	

	if (EvalTargetFunc(hitResult))
	{
		DrawDebugLine(world, targetBegin, targetEnd, FColor::Green);
	}
	else
	{
		DrawDebugLine(world, targetBegin, targetEnd, FColor::Red);
	}
}

bool AGameplayAbilityTargetActor_Raycast::IsConfirmTargetingAllowed()
{
	return true;
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
			TargetDataReadyDelegate.Broadcast(makeDataHandle());
		else
			CancelTargeting();
	}
}

