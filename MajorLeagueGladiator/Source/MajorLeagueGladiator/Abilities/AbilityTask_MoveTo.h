// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Abilities/Tasks/AbilityTask.h"
#include "AbilityTask_MoveTo.generated.h"

DECLARE_MULTICAST_DELEGATE(FAbilityTaskMoveToLocationReached);

UCLASS()
class MAJORLEAGUEGLADIATOR_API UAbilityTask_MoveTo : public UAbilityTask
{
	GENERATED_BODY()


public:
	static UAbilityTask_MoveTo* Create(UGameplayAbility* ThisAbility, FName TaskName, FVector TargetLocation, float MoveSpeed, ACharacter* MovingCharacter);

	UAbilityTask_MoveTo();

	virtual void Activate() override;
	virtual void TickTask(float DeltaTime) override;
	virtual void OnDestroy(bool AbilityEnded) override;

	FAbilityTaskMoveToLocationReached OnLocationReached;

private:
	UPROPERTY()
	ACharacter* MovingCharacter;

	UPROPERTY()
	FVector TargetLocation;

	UPROPERTY()
	float MoveSpeed;

	UPROPERTY()
	float MinDistanceThreshold;

	UCharacterMovementComponent* cachedMoveComp;
};
