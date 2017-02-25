// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Abilities/GameplayAbility.h"
#include "DashAbility.generated.h"

class UAbilityTask_MoveTo;
class UAbilityTask_WaitTargetData;
class AGameplayAbilityTargetActor_Raycast;

UCLASS()
class MAJORLEAGUEGLADIATOR_API UDashAbility : public UGameplayAbility
{
	GENERATED_BODY()

public:
	UDashAbility();

	UPROPERTY()
	float MaxRange;

private:
	virtual void InputReleased(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo) override;
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;

	void OnLocationReached();

	UFUNCTION()
	void OnTargetPickSuccessful(const FGameplayAbilityTargetDataHandle& Data);

	UFUNCTION()
	void OnTargetPickCanceled(const FGameplayAbilityTargetDataHandle& Data);

	UPROPERTY(Transient)
	UAbilityTask_WaitTargetData* waitForTargetTask;

	UPROPERTY(Transient)
	UAbilityTask_MoveTo* moveToTask;

	UPROPERTY(Transient)
	AGameplayAbilityTargetActor_Raycast* targetingSpawnedActor;
};