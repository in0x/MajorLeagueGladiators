// Fill out your copyright notice in the Description page of Project Settings.

#include "MajorLeagueGladiator.h"
#include "JumpDashAbility.h"

#include "AbilityTask_WaitTargetData.h"
#include "Abilities/GameplayAbilityTargetActor_Raycast.h"

#include "Characters/MlgPlayerCharacter.h"

namespace
{
	const char* AIM_SOCKET_NAME = "Aim";
}

UJumpDashAbility::UJumpDashAbility()
	: launchVelocity(0,0,1300)
	, minmalJumpHightBeforeDash(500)
	, dashSpeed(1750)
	, maxDashRange(50'000)
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

	cachedCharacter->LaunchCharacter(launchVelocity, true, true);
	cachedCharacter->MovementModeChangedDelegate.AddDynamic(this, &UJumpDashAbility::OnMovementModeChanged);

	const float targetingDelay = minmalJumpHightBeforeDash / launchVelocity.Z;

	FTimerManager& timeManager = cachedCharacter->GetWorldTimerManager();
	timeManager.SetTimer(timerHandle, this, &UJumpDashAbility::BeginTargeting,
		targetingDelay, false);
}

void UJumpDashAbility::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);

	FTimerManager& timeManager = cachedCharacter->GetWorldTimerManager();
	timeManager.ClearTimer(timerHandle);
	timerHandle.Invalidate();

	cachedCharacter->MovementModeChangedDelegate.RemoveDynamic(this, &UJumpDashAbility::OnMovementModeChanged);
	
	if (waitTargetDataTask)
	{
		waitTargetDataTask->EndTask();
		waitTargetDataTask = nullptr;
	}
}

void UJumpDashAbility::OnMovementModeChanged(ACharacter* Character, EMovementMode PrevMovementMode, uint8 PreviousCustomMode)
{	
	if (cachedMoveComp->MovementMode == MOVE_Walking)
	{
		OnLanded();
	}	
}


void UJumpDashAbility::OnLanded()
{
	cachedMoveComp->StopMovementImmediately();
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
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

	BeginDashing(velocity);
}

void UJumpDashAbility::OnTargetingFailed(const FGameplayAbilityTargetDataHandle& Data)
{
	BeginDashing(FVector(0, 0, -dashSpeed));
}

void UJumpDashAbility::BeginDashing(const FVector& Velocity)
{
	cachedMoveComp->StopMovementImmediately();
	cachedMoveComp->SetMovementMode(MOVE_Custom);
	cachedMoveComp->Launch(Velocity);
}

