// Fill out your copyright notice in the Description page of Project Settings.

#include "MajorLeagueGladiator.h"
#include "JumpDashAbility.h"

#include "AbilityTask_WaitMovementModeChange.h"
#include "AbilityTask_WaitTargetData.h"
#include "Abilities/GameplayAbilityTargetActor_Raycast.h"

#include "Characters/MlgPlayerCharacter.h"
#include "Abilities/AbilityTask_DashTo.h"
#include "Abilities/AbilityTask_MoveTo.h"
#include "Characters/MlgPlayerCharacter.h"

namespace
{
	const char* AIM_SOCKET_NAME = "Aim";
}

UJumpDashAbility::UJumpDashAbility()
	: launchVelocity(0,0,1300)
	, dashSpeed(1300)
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

	cachedCharacter = CastChecked<ACharacter>(ActorInfo->AvatarActor.Get());
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

	BeginTargeting();
}

void UJumpDashAbility::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);

	cachedCharacter->MovementModeChangedDelegate.RemoveDynamic(this, &UJumpDashAbility::OnMovementModeChanged);

	if (waitTargetDataTask)
	{
		waitTargetDataTask->EndTask();
		waitTargetDataTask = nullptr;
	}

	if (dashTask)
	{
		dashTask->EndTask();
		dashTask = nullptr;
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

	auto player = CastChecked<AMlgPlayerCharacter>(GetOwningActorFromActorInfo());
	auto gripController = player->GetMotionControllerMesh(EControllerHand::Left);
	targetingRayCastActor->StartLocation.SourceComponent = gripController;
	targetingRayCastActor->StartLocation.LocationType = EGameplayAbilityTargetingLocationType::SocketTransform;
	targetingRayCastActor->StartLocation.SourceSocketName = AIM_SOCKET_NAME;

	targetingRayCastActor->aimDirection = ERaycastTargetDirection::ComponentRotation;
	targetingRayCastActor->IgnoredActors.Add(GetOwningActorFromActorInfo());
	targetingRayCastActor->MaxRange = 50000;

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
	const FVector actorLocation = GetOwningActorFromActorInfo()->GetActorLocation();

	const FVector distance = targetLocation - actorLocation;

	BeginDashing(distance.GetUnsafeNormal() * dashSpeed);
}

void UJumpDashAbility::OnTargetingFailed(const FGameplayAbilityTargetDataHandle& Data)
{
	BeginDashing(FVector(0, 0, -dashSpeed));
}

void UJumpDashAbility::BeginDashing(const FVector& Velocity)
{
	dashTask = UAbilityTask_DashTo::Create(this, TEXT("DashTo"), Velocity, 5);
	dashTask->ReadyForActivation();
}

