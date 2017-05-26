// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Abilities/GameplayAbility.h"
#include "JumpDashAbility.generated.h"

class UAbilityTask_WaitTargetData;
class AGameplayAbilityTargetActor;
class AMlgPlayerCharacter;
class UAbilityTask_MoveTo;

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
	float minmalJumpHightBeforeDash;

	UPROPERTY(EditAnywhere)
	float dashSpeed;

	UPROPERTY(EditAnywhere)
	float maxDashRange;

	UPROPERTY(EditAnywhere)
	float effectDistance;

	UPROPERTY(EditAnywhere)
	float halfEffectAngleDegrees;

	UPROPERTY(EditAnywhere)
	float stunRadius;

	UPROPERTY(EditAnywhere)
	float stunTimeSeconds;

	UPROPERTY(EditAnywhere)
	UParticleSystem* jumpParticleEffect;

	UPROPERTY(EditAnywhere)
	UParticleSystem* landingParticleEffect;

	UPROPERTY(EditAnywhere)
	USoundBase* landingSoundEffect;

	UPROPERTY(EditAnywhere)
	USoundCue* jumpSoundCue;

	FTimerHandle timerHandle;

	UPROPERTY(Transient)
	UAbilityTask_WaitTargetData* waitTargetDataTask;

	UPROPERTY(Transient)
	AGameplayAbilityTargetActor* spawnedTargetingActor_dash;

	UPROPERTY(Transient)
	UAbilityTask_MoveTo* moveToTask;

	UPROPERTY(Transient)
	AMlgPlayerCharacter* cachedCharacter;

	UPROPERTY(Transient)
	UCharacterMovementComponent* cachedMoveComp;

	void BeginFindingActorsToLaunch();

	bool CanLaunchCharacter(ACharacter* Character) const;

	void BeginTargeting();

	void DashTo(const FVector& Location, float MoveSpeed);

	UFUNCTION()
	void OnLocationReached();

	UFUNCTION()
	void LaunchFoundActorsUpwards(const FGameplayAbilityTargetDataHandle& Data);

	UFUNCTION()
	void OnTargetingSuccess(const FGameplayAbilityTargetDataHandle& Data);

	UFUNCTION()
	void OnMovementModeChanged(class ACharacter* Character, EMovementMode PrevMovementMode, uint8 PreviousCustomMode);

	void StunEnemiesInArea();

	void PlayJumpEffects();

	void PlayLandingEffects();

	void SetSwordAlwaysFastEnough(bool IsAlwaysFastEnough);
};
