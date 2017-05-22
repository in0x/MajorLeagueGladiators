// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Abilities/Tasks/AbilityTask.h"
#include "AbilityTask_AddMovementInput.generated.h"

UCLASS()
class MAJORLEAGUEGLADIATOR_API UAbilityTask_AddMovementInput : public UAbilityTask
{
	GENERATED_BODY()
	
public:
	static UAbilityTask_AddMovementInput* Create(
		UGameplayAbility* ThisAbility,
		FName TaskName,
		APawn* MovingCharacter,
		USceneComponent* GetDirectionFromComponent);
	
	UAbilityTask_AddMovementInput();
	virtual void TickTask(float DeltaTime) override;
	
private:
	UPROPERTY()
	APawn* movingCharacter;

	UPROPERTY()
	USceneComponent* getDirectionFromComponent;
};
