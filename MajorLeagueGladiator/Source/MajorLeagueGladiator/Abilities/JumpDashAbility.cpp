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
	: LaunchVelocity(0,0,1000)
{

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

	ACharacter* character = CastChecked<ACharacter>(ActorInfo->AvatarActor.Get());

	if (character->GetReplicatedMovementMode() != MOVE_Walking)
	{
		return;
	}

	if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
	{
		UE_LOG(LogTemp, Warning, TEXT("Tried to activate ability with insufficient resources"));
		return;
	}


	character->LaunchCharacter(LaunchVelocity, true, true);

	character->MovementModeChangedDelegate.AddDynamic(this, &UJumpDashAbility::OnMovementModeChanged);

	BeginTargeting();
}

void UJumpDashAbility::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
	ACharacter* character = CastChecked<ACharacter>(ActorInfo->AvatarActor.Get());
	character->MovementModeChangedDelegate.RemoveDynamic(this, &UJumpDashAbility::OnMovementModeChanged);

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
	UCharacterMovementComponent *moveComp = CastChecked<UCharacterMovementComponent>(Character->GetMovementComponent());
	
	if (moveComp->MovementMode == MOVE_Walking)
	{
		OnLanded();
	}	
}


void UJumpDashAbility::OnLanded()
{
	ACharacter* character = CastChecked<ACharacter>(CurrentActorInfo->AvatarActor.Get());
	character->GetMovementComponent()->StopMovementImmediately();
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}

void UJumpDashAbility::BeginTargeting()
{
	waitTargetDataTask = UAbilityTask_WaitTargetData::WaitTargetData(this, "WaitTargetDataTask",
		EGameplayTargetingConfirmation::Custom, AGameplayAbilityTargetActor_Raycast::StaticClass());

	waitTargetDataTask->ValidData.AddDynamic(this, &UJumpDashAbility::OnTargetingSuccess);
	waitTargetDataTask->ValidData.AddDynamic(this, &UJumpDashAbility::OnTargetingFailed);

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

	dashTask = UAbilityTask_MoveTo::Create(this, TEXT("MoveTo"), targetLocation, 1000, CastChecked<AMlgPlayerCharacter>(CurrentActorInfo->AvatarActor.Get()));
	//dashTask = UAbilityTask_DashTo::Create(this, TEXT("DashTo"), distance.GetUnsafeNormal() * 1000, 5);
	dashTask->ReadyForActivation();
}

void UJumpDashAbility::OnTargetingFailed(const FGameplayAbilityTargetDataHandle& Data)
{
}
