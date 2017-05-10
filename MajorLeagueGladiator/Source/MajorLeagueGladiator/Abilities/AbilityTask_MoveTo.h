// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Abilities/Tasks/AbilityTask.h"
#include "AbilityTask_MoveTo.generated.h"

class AMlgPlayerCharacter;

DECLARE_MULTICAST_DELEGATE(FAbilityTaskMoveToLocationReached);

UCLASS()
class MAJORLEAGUEGLADIATOR_API UAbilityTask_MoveTo : public UAbilityTask
{
	GENERATED_BODY()

public:
	static UAbilityTask_MoveTo* Create(
		UGameplayAbility* ThisAbility,
		FName TaskName,
		FVector TargetLocation,
		float MoveSpeed,
		AMlgPlayerCharacter* MovingCharacter,
		bool ShouldAdjustHight = true);

	UAbilityTask_MoveTo();

	virtual void Activate() override;
	virtual void TickTask(float DeltaTime) override;
	virtual void OnDestroy(bool AbilityEnded) override;

	FAbilityTaskMoveToLocationReached OnLocationReached;

	void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty> & OutLifetimeProps) const override;
	
private:
	void OnTimeout();

	UPROPERTY(replicated)
	AMlgPlayerCharacter* movingCharacter;

	UPROPERTY(replicated)
	FVector targetLocation;

	UPROPERTY(replicated)
	float moveSpeed;

	UPROPERTY(replicated)
	UCharacterMovementComponent* cachedMoveComp;

	FTimerHandle timeout;
};
