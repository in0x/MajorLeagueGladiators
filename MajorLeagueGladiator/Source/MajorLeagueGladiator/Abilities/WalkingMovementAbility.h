// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Abilities/GameplayAbility.h"
#include "WalkingMovementAbility.generated.h"

class UAbilityTask_AddMovementInput;

UCLASS()
class MAJORLEAGUEGLADIATOR_API UWalkingMovementAbility : public UGameplayAbility
{
	GENERATED_BODY()
public:
	UWalkingMovementAbility(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

private:
	virtual void InputReleased(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo) override;
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	virtual void UWalkingMovementAbility::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;

	UPROPERTY(EditAnywhere)
	EControllerHand targetingHand;

	UPROPERTY(Transient)
	UAbilityTask_AddMovementInput* movementTask;
};
