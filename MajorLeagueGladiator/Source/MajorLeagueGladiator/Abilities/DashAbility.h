// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Abilities/GameplayAbility.h"
#include "DashAbility.generated.h"

class UAbilityTask_WaitTargetData;
class AGameplayAbilityTargetActor_PredictProjectile;

UCLASS()
class MAJORLEAGUEGLADIATOR_API UDashAbility : public UGameplayAbility
{
	GENERATED_BODY()

public:
	UDashAbility();

private:

};