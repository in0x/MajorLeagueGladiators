// Fill out your copyright notice in the Description page of Project Settings.

#include "MajorLeagueGladiator.h"
#include "AbilityTask_AddMovementInput.h"

UAbilityTask_AddMovementInput* UAbilityTask_AddMovementInput::Create(
	UGameplayAbility* ThisAbility,
	FName TaskName,
	ACharacter* MovingCharacter,
	USceneComponent* GetDirectionFromComponent)
{
	UAbilityTask_AddMovementInput* task = NewAbilityTask<UAbilityTask_AddMovementInput>(ThisAbility, TaskName);
	task->movingCharacter = MovingCharacter;
	task->getDirectionFromComponent = GetDirectionFromComponent;
	
	return task;
}

UAbilityTask_AddMovementInput::UAbilityTask_AddMovementInput()
{
	bTickingTask = true;
}

void UAbilityTask_AddMovementInput::TickTask(float DeltaTime)
{
	Super::TickTask(DeltaTime);

	if (movingCharacter->GetCharacterMovement()->MovementMode != EMovementMode::MOVE_Walking)
	{
		OnCancelAbility.Broadcast();
		return;
	}

	FVector forward = getDirectionFromComponent->GetForwardVector();
	forward.Normalize();
	FVector direction(forward.X, forward.Y, 0.f);

	movingCharacter->AddMovementInput(direction, 1.f);
}



