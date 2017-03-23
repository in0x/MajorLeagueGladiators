// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Abilities/Tasks/AbilityTask.h"
#include "AbilityTask_DashTo.generated.h"

DECLARE_MULTICAST_DELEGATE_OneParam(OnCollisionDelegate, const FHitResult&);

UCLASS()
class MAJORLEAGUEGLADIATOR_API UAbilityTask_DashTo : public UAbilityTask
{
	GENERATED_BODY()
public:
	UAbilityTask_DashTo();

	static UAbilityTask_DashTo* Create(UGameplayAbility* ThisAbility, FName TaskName, const FVector& Velocity, float TimeInSeconds);

	virtual void Activate() override;
	virtual void TickTask(float DeltaTime) override;

	virtual void OnDestroy(bool bInOwnerFinished) override;

	FVector CalcMovementDelta(float DeltaTime) const;
	
	OnCollisionDelegate OnCollision;

private:
	FVector velocity;
	float time;

	UPROPERTY(Transient)
	UPrimitiveComponent* cachedPrimitive;

	UPROPERTY(Transient)
	UCharacterMovementComponent* cachedMovementComponent;
};
