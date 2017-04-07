// Fill out your copyright notice in the Description page of Project Settings.

#include "MajorLeagueGladiator.h"
#include "JumpDashAbility.h"

#include "AbilityTask_WaitTargetData.h"
#include "Abilities/GameplayAbilityTargetActor_Raycast.h"
#include "Abilities/GameplayAbilityTargetActor_Cone.h"

#include "Characters/MlgPlayerCharacter.h"

#include "MlgGameplayStatics.h"

namespace
{
	const char* AIM_SOCKET_NAME = "Aim";
}

UJumpDashAbility::UJumpDashAbility()
	: launchVelocity(0,0,1300)
	, minmalJumpHightBeforeDash(500)
	, dashSpeed(1750)
	, maxDashRange(50'000)
	, effectDistance(400)
	, halfEffectAngleDegrees(45)
{
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalPredicted;
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerExecution;
}

void UJumpDashAbility::InputPressed(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo)
{
	if (waitTargetDataTask)
	{
		waitTargetDataTask->ExternalConfirm(true);
		waitTargetDataTask = nullptr;
	}
}

void UJumpDashAbility::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	if (!ActorInfo->AvatarActor.IsValid())
	{
		UE_LOG(LogTemp, Warning, TEXT("Invalid Avatar"));
		return;
	}

	cachedCharacter = CastChecked<AMlgPlayerCharacter>(ActorInfo->AvatarActor.Get());
	cachedMoveComp = CastChecked<UCharacterMovementComponent>(cachedCharacter->GetMovementComponent());

	if (cachedMoveComp->MovementMode != MOVE_Walking)
	{
		EndAbility(Handle,ActorInfo, ActivationInfo, true, true);
		return;
	}

	if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
	{
		UE_LOG(LogTemp, Warning, TEXT("Tried to activate ability with insufficient resources"));
		return;
	}

	cachedCharacter->SetAbilityMoveTargetLocation(cachedCharacter->GetActorLocation());
	cachedCharacter->LaunchCharacter(launchVelocity, true, true);
	cachedCharacter->OnActorHit.AddDynamic(this, &UJumpDashAbility::OnCollidedWithWorld);
	BeginFindingActorsToLaunch();

	const float targetingDelay = minmalJumpHightBeforeDash / launchVelocity.Z;

	FTimerManager& timeManager = cachedCharacter->GetWorldTimerManager();
	timeManager.SetTimer(timerHandle, this, &UJumpDashAbility::BeginTargeting,
		targetingDelay, false);
}

void UJumpDashAbility::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);

	if (bWasCancelled)
	{
		return;
	}

	cachedMoveComp->SetMovementMode(MOVE_Falling);
	cachedMoveComp->StopMovementImmediately();

	FTimerManager& timeManager = cachedCharacter->GetWorldTimerManager();
	timeManager.ClearTimer(timerHandle);
	timerHandle.Invalidate();

	cachedCharacter->OnActorHit.RemoveDynamic(this, &UJumpDashAbility::OnCollidedWithWorld);
	
	if (waitTargetDataTask)
	{
		waitTargetDataTask->EndTask();
		waitTargetDataTask = nullptr;
	}

	cachedCharacter->InvalidateAbilityMoveTargetLocation();
}

void UJumpDashAbility::OnCollidedWithWorld(AActor* SelfActor, AActor* OtherActor, FVector NormalImpulse, const FHitResult& Hit)
{	
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}

void UJumpDashAbility::BeginFindingActorsToLaunch()
{
	waitTargetDataTask = UAbilityTask_WaitTargetData::WaitTargetData(this, "WaitTargetDataTask",
		EGameplayTargetingConfirmation::Custom, AGameplayAbilityTargetActor_Cone::StaticClass());

	waitTargetDataTask->ValidData.AddDynamic(this, &UJumpDashAbility::LaunchFoundActorsUpwards);

	AGameplayAbilityTargetActor* spawnedTargetingActor = nullptr;

	if (!waitTargetDataTask->BeginSpawningActor(this, AGameplayAbilityTargetActor_Cone::StaticClass(), spawnedTargetingActor))
	{
		return;
	}

	AGameplayAbilityTargetActor_Cone* targetingConeActor = CastChecked<AGameplayAbilityTargetActor_Cone>(spawnedTargetingActor);
	targetingConeActor->Distance = effectDistance;
	targetingConeActor->HalfAngleDegrees = halfEffectAngleDegrees;
}

void UJumpDashAbility::LauchNearEnemies()
{
	const TArray<TEnumAsByte<EObjectTypeQuery>> queryTypes{
		UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_Pawn)
	};

	UWorld* world = cachedCharacter->GetWorld();

	const FVector actorLocation = cachedCharacter->GetActorLocation();
	const FVector2D forwardVector(cachedCharacter->GetVRForwardVector());
	const FVector2D normalizedForwardVector = forwardVector.GetSafeNormal();

	DrawDebugSphere(world, actorLocation, effectDistance, 16, FColor::Silver, true, 2.f);


	TArray<FOverlapResult> outOverlapResult;
	world->GetWorld()->OverlapMultiByObjectType(
		outOverlapResult,
		actorLocation,
		FQuat::Identity, 
		FCollisionObjectQueryParams(queryTypes),
		FCollisionShape::MakeSphere(effectDistance)
	);

	for (FOverlapResult& result : outOverlapResult)
	{
		if(ACharacter* character = Cast<ACharacter>(result.Actor.Get()))
		{
			const FVector2D distance(character->GetActorLocation() - cachedCharacter->GetActorLocation());
			const FVector2D normalizedDistance = distance.GetSafeNormal();

			const float angleRadiens = acosf(FVector2D::DotProduct(normalizedDistance, normalizedForwardVector));
			const float angleDegrees = FMath::RadiansToDegrees(angleRadiens);

			if (angleDegrees < halfEffectAngleDegrees && CanLaunchCharacter(character))
			{
				character->LaunchCharacter(launchVelocity, true, true);
			}
		}
	}
}

bool UJumpDashAbility::CanLaunchCharacter(ACharacter* Character) const
{
	return UMlgGameplayStatics::CanDealDamageTo(cachedCharacter, Character);
}

void UJumpDashAbility::BeginTargeting()
{
	waitTargetDataTask = UAbilityTask_WaitTargetData::WaitTargetData(this, "WaitTargetDataTask",
		EGameplayTargetingConfirmation::Custom, AGameplayAbilityTargetActor_Raycast::StaticClass());

	waitTargetDataTask->ValidData.AddDynamic(this, &UJumpDashAbility::OnTargetingSuccess);
	waitTargetDataTask->Cancelled.AddDynamic(this, &UJumpDashAbility::OnTargetingFailed);

	AGameplayAbilityTargetActor* spawnedTargetingActor = nullptr;

	if (!waitTargetDataTask->BeginSpawningActor(this, AGameplayAbilityTargetActor_Raycast::StaticClass(), spawnedTargetingActor))
	{
		return;
	}

	AGameplayAbilityTargetActor_Raycast* targetingRayCastActor = CastChecked<AGameplayAbilityTargetActor_Raycast>(spawnedTargetingActor);

	
	auto gripController = cachedCharacter->GetMotionControllerMesh(EControllerHand::Left);
	targetingRayCastActor->StartLocation.SourceComponent = gripController;
	targetingRayCastActor->StartLocation.LocationType = EGameplayAbilityTargetingLocationType::SocketTransform;
	targetingRayCastActor->StartLocation.SourceSocketName = AIM_SOCKET_NAME;

	targetingRayCastActor->aimDirection = ERaycastTargetDirection::ComponentRotation;
	targetingRayCastActor->IgnoredActors.Add(cachedCharacter);
	targetingRayCastActor->MaxRange = maxDashRange;

	targetingRayCastActor->EvalTargetFunc = [](const FHitResult& result)
	{
		auto isNotVertical = FVector::DotProduct(result.ImpactNormal, FVector(0, 0, 1)) > 0.7f;
		auto hitSurface = result.bBlockingHit > 0;
		return isNotVertical && hitSurface;
	};

	waitTargetDataTask->FinishSpawningActor(this, spawnedTargetingActor);
}

void UJumpDashAbility::OnTargetingSuccess(const FGameplayAbilityTargetDataHandle& Data)
{
	const FVector targetLocation = Data.Data[0]->GetHitResult()->Location;
	const FVector actorFeetLocation = cachedCharacter->CalcFeetPosition();
	const FVector feetToTargetVector = targetLocation - actorFeetLocation;
	const FVector direction = feetToTargetVector.GetUnsafeNormal();
	const FVector zNormalizedDirection = direction / std::abs(direction.Z);
	const FVector velocity = zNormalizedDirection * dashSpeed;
	
	cachedCharacter->SetAbilityMoveTargetLocation(targetLocation);
	BeginDashing(velocity);
}

void UJumpDashAbility::OnTargetingFailed(const FGameplayAbilityTargetDataHandle& Data)
{
	BeginDashing(FVector(0, 0, -dashSpeed));
}

void UJumpDashAbility::BeginDashing(const FVector& Velocity)
{
	cachedMoveComp->StopMovementImmediately();
	cachedMoveComp->SetMovementMode(MOVE_Flying);
	cachedMoveComp->AddImpulse(Velocity, true);
}

void UJumpDashAbility::LaunchFoundActorsUpwards(const FGameplayAbilityTargetDataHandle& Data)
{
	const FGameplayAbilityTargetData* targetData = Data.Get(0);
	for (const auto& actor : targetData->GetActors())
	{
		if (actor.IsValid())
		{
			ACharacter* characterToLaunch = CastChecked<ACharacter>(actor.Get());
			characterToLaunch->LaunchCharacter(launchVelocity, true, true);
		}
	}	
}


