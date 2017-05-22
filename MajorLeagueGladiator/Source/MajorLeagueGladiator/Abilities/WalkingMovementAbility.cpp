// Fill out your copyright notice in the Description page of Project Settings.

#include "MajorLeagueGladiator.h"
#include "WalkingMovementAbility.h"
#include "AbilityTask_AddMovementInput.h"
#include "Characters/MlgPlayerCharacter.h"
#include "VRControllerComponent.h"

UWalkingMovementAbility::UWalkingMovementAbility(const FObjectInitializer& ObjectInitializer)
{
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalOnly;
}

void UWalkingMovementAbility::InputReleased(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo)
{
	movementTask->EndTask();
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, false, false);
}

void UWalkingMovementAbility::OnMovementTaskCancel()
{
	movementTask->EndTask();
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, false, false);
}

void UWalkingMovementAbility::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	AMlgPlayerCharacter* owningChar = CastChecked<AMlgPlayerCharacter>(GetOwningActorFromActorInfo());

	movementTask = UAbilityTask_AddMovementInput::Create(this, "AddMovementInputTask", owningChar, owningChar->GetMotionController(targetingHand));
	movementTask->OnCancelAbility.AddDynamic(this, &UWalkingMovementAbility::OnMovementTaskCancel);
	movementTask->ReadyForActivation();
	CommitAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo);
}

void UWalkingMovementAbility::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
	movementTask = nullptr;
}


