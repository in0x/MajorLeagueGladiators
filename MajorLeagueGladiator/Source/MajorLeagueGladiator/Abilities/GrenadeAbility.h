// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Abilities/GameplayAbility.h"
#include "GrenadeAbility.generated.h"

class UAbilityTask_WaitTargetData;
class AMlgPlayerCharacter;
class AGrenadeProjectile;

UCLASS()
class MAJORLEAGUEGLADIATOR_API UGrenadeAbility : public UGameplayAbility
{
	GENERATED_BODY()
public:
	UGrenadeAbility();

	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	virtual void InputReleased(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo) override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;

private:
	UPROPERTY(Transient)
	UAbilityTask_WaitTargetData* waitTargetDataTask;
	
	UPROPERTY(Transient)
	AGrenadeProjectile* grenadeDefaultObject;

	UPROPERTY(Transient)
	AMlgPlayerCharacter* cachedPlayer;

	void beginTargeting();
	void fireGrenade();

	UFUNCTION()
	void onTargetingSuccess(const FGameplayAbilityTargetDataHandle& data);

	UFUNCTION()
	void onTargetingFailed(const FGameplayAbilityTargetDataHandle& data);
};
