// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Abilities/GameplayAbility.h"
#include "JumpAbility.generated.h"

class UAbilityTask_WaitTargetData;
class AGameplayAbilityTargetActor_PredictProjectile;

UCLASS()
class MAJORLEAGUEGLADIATOR_API UJumpAbility : public UGameplayAbility
{
	GENERATED_BODY()

public:
	UJumpAbility();

	// The speed with which the targeting projectile will be launched.
	UPROPERTY(EditAnywhere, Category = "JumpAbility")
	float PredictProjectileSpeed;

	UPROPERTY(EditAnywhere, Category = "JumpAbility")
	float MaxTimeInFlight;

private:
	virtual void InputReleased(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo) override;
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;

	UFUNCTION()
	void OnTargetPickSuccessful(const FGameplayAbilityTargetDataHandle& Data);

	UFUNCTION()
	void OnTargetPickCanceled(const FGameplayAbilityTargetDataHandle& Data);

	UFUNCTION()
	void OnMovementModeChanged(ACharacter* Character, EMovementMode PrevMovementMode, uint8 PreviousCustomMode);

	UPROPERTY(Transient)
	UAbilityTask_WaitTargetData* waitForTargetTask;

	UPROPERTY(Transient)
	AGameplayAbilityTargetActor_PredictProjectile* targetingSpawnedActor;
};