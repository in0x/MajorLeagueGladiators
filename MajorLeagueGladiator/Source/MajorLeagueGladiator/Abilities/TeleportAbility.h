// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Abilities/GameplayAbility.h"
#include "TeleportAbility.generated.h"

class UAbilityTask_WaitTargetData;
class AGameplayAbilityTargetActor_PredictProjectile;

UCLASS()
class MAJORLEAGUEGLADIATOR_API UTeleportAbility : public UGameplayAbility
{
	GENERATED_BODY()

public:
	UTeleportAbility();

	// The speed with which the targeting projectile will be launched.
	UPROPERTY()
	float PredictProjectileSpeed;

private:
	virtual void InputReleased(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo) override;
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;

	UFUNCTION()
	void OnTargetPickSuccessful(const FGameplayAbilityTargetDataHandle& Data);

	UFUNCTION()
	void OnTargetPickCanceled(const FGameplayAbilityTargetDataHandle& Data);

	UPROPERTY(Transient)
	UAbilityTask_WaitTargetData* waitForTargetTask;

	UPROPERTY(Transient)
	AGameplayAbilityTargetActor_PredictProjectile* targetingSpawnedActor;
};