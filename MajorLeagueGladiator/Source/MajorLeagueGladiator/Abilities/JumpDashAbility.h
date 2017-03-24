// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Abilities/GameplayAbility.h"
#include "JumpDashAbility.generated.h"

class UAbilityTask_WaitTargetData;
class AGameplayAbilityTargetActor;

UCLASS()
class MAJORLEAGUEGLADIATOR_API UJumpDashAbility : public UGameplayAbility
{
	GENERATED_BODY()
public:
	UJumpDashAbility();

	virtual void InputPressed(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo) override;
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;

private:
	UPROPERTY(EditAnywhere)
	FVector launchVelocity;

	UPROPERTY(EditAnywhere)
	float dashSpeed;

	UPROPERTY(EditAnywhere)
	float maxDashRange;

	UPROPERTY(Transient)
	UAbilityTask_WaitTargetData* waitTargetDataTask;

	UPROPERTY(Transient)
	UCharacterMovementComponent* cachedMoveComp;

	UPROPERTY(Transient)
	ACharacter* cachedCharacter;

	void BeginTargeting();

	void BeginDashing(const FVector& Velocity);

	UFUNCTION()
	void OnMovementModeChanged(ACharacter* Character, EMovementMode PrevMovementMode, uint8 PreviousCustomMode);

	void OnLanded();

	UFUNCTION()
	void OnTargetingSuccess(const FGameplayAbilityTargetDataHandle& Data);

	UFUNCTION()
	void OnTargetingFailed(const FGameplayAbilityTargetDataHandle& Data);	
};
