// Fill out your copyright notice in the Description page of Project Settings.

#include "MajorLeagueGladiator.h"
#include "AbilityTask_MoveTo.h"

UAbilityTask_MoveTo* UAbilityTask_MoveTo::Create(UGameplayAbility* ThisAbility, FName TaskName, FVector TargetLocation, float MoveSpeed, ACharacter* MovingCharacter, float MinDistanceThreshold)
{
	UAbilityTask_MoveTo* task = NewAbilityTask<UAbilityTask_MoveTo>(ThisAbility, TaskName);
	task->MovingCharacter = MovingCharacter;
	task->TargetLocation = TargetLocation;
	task->MinDistanceThreshold = MinDistanceThreshold;
	task->MoveSpeed = MoveSpeed;
	task->cachedMoveComp = CastChecked<UVRBaseCharacterMovementComponent>(MovingCharacter->GetCharacterMovement());

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

	const FVector location = MovingCharacter->GetActorLocation();
	const float distance = FVector::Distance(TargetLocation, location);

	DrawDebugLine(MovingCharacter->GetRootComponent()->GetWorld(), location, TargetLocation, FColor::Green, false,-1,0,5);

	if (distance < MinDistanceThreshold)
	{
		EndTask();
		OnLocationReached.Broadcast();
	}
	else
	{
		const FVector Velocity = (TargetLocation - location).GetSafeNormal() * MoveSpeed;
		cachedMoveComp->MoveSmooth(Velocity, DeltaTime);
	}
}


void UAbilityTask_MoveTo::OnDestroy(bool AbilityEnded)
{
	Super::OnDestroy(AbilityEnded);

	cachedMoveComp->SetMovementMode(MOVE_Falling);
	cachedMoveComp->StopMovementImmediately();
}

void UAbilityTask_MoveTo::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UAbilityTask_MoveTo, MovingCharacter);
	DOREPLIFETIME(UAbilityTask_MoveTo, TargetLocation);
	DOREPLIFETIME(UAbilityTask_MoveTo, MoveSpeed);
	DOREPLIFETIME(UAbilityTask_MoveTo, MinDistanceThreshold);
	DOREPLIFETIME(UAbilityTask_MoveTo, cachedMoveComp);
}

