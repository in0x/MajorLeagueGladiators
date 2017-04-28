// Fill out your copyright notice in the Description page of Project Settings.

#include "MajorLeagueGladiator.h"
#include "AbilityTask_MoveTo.h"
#include "AbilityTask_MoveToActor.h"

UAbilityTask_MoveTo* UAbilityTask_MoveTo::Create(UGameplayAbility* ThisAbility, FName TaskName, FVector TargetLocation, float MoveSpeed, ACharacter* MovingCharacter)
{
	UAbilityTask_MoveTo* task = NewAbilityTask<UAbilityTask_MoveTo>(ThisAbility, TaskName);
	task->MovingCharacter = MovingCharacter;
	task->TargetLocation = TargetLocation;
	task->MinDistanceThreshold = 50.f;
	task->MoveSpeed = MoveSpeed;
	task->cachedMoveComp = CastChecked<UCharacterMovementComponent>(MovingCharacter->GetMovementComponent());

	return task;
}

UAbilityTask_MoveTo::UAbilityTask_MoveTo()
{
	bTickingTask = true;
	bSimulatedTask = true;
}

void UAbilityTask_MoveTo::Activate()
{
	Super::Activate();

	cachedMoveComp->SetMovementMode(MOVE_Flying);
}

void UAbilityTask_MoveTo::TickTask(float DeltaTime)
{
	Super::TickTask(DeltaTime);

	if (!MovingCharacter->HasAuthority()) { return; }

	const FVector location = MovingCharacter->GetActorLocation();
	const float distance = FVector::Distance(TargetLocation, location); // This needs to happen locally

	if (distance < MinDistanceThreshold)
	{
		EndTask();
		OnLocationReached.Broadcast();
	}
	else
	{
		const FVector Velocity = (TargetLocation - location).GetSafeNormal() * MoveSpeed;

		cachedMoveComp->MoveSmooth(Velocity, DeltaTime);

		DrawDebugDirectionalArrow(MovingCharacter->GetRootComponent()->GetWorld(), location, TargetLocation, 1.0f, FColor::Green);
	}
}


void UAbilityTask_MoveTo::OnDestroy(bool AbilityEnded)
{
	Super::OnDestroy(AbilityEnded);

	cachedMoveComp->SetMovementMode(MOVE_Falling);
	cachedMoveComp->StopMovementImmediately();
}
