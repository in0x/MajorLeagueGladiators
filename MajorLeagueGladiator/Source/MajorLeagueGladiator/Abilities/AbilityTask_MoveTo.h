// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Abilities/Tasks/AbilityTask.h"
#include "AbilityTask_MoveTo.generated.h"

class AAbilityTask_MoveToActor;

DECLARE_MULTICAST_DELEGATE(FAbilityTaskMoveToLocationReached);

UCLASS()
class MAJORLEAGUEGLADIATOR_API UAbilityTask_MoveTo : public UAbilityTask
{
	GENERATED_BODY()

public:
	static UAbilityTask_MoveTo* Create(UGameplayAbility* ThisAbility, FName TaskName, FVector TargetLocation, float MoveSpeed, ACharacter* MovingCharacter);

	virtual void Activate() override;
	void OnDestroy(bool AbilityEnded) override;

	FAbilityTaskMoveToLocationReached OnLocationReached;

private:
	AAbilityTask_MoveToActor* spawnedActor;

	void OnLocationReachedCallback();
};
