// Fill out your copyright notice in the Description page of Project Settings.

#include "MajorLeagueGladiator.h"
#include "AbilityTask_MoveTo.h"
#include "Characters/MlgPlayerCharacter.h"


UAbilityTask_MoveTo* UAbilityTask_MoveTo::Create(
	UGameplayAbility* ThisAbility,
	FName TaskName,
	FVector TargetLocation,
	float MoveSpeed,
	AMlgPlayerCharacter* MovingCharacter,
	bool ShouldAdjustHight)
{
	UAbilityTask_MoveTo* task = NewAbilityTask<UAbilityTask_MoveTo>(ThisAbility, TaskName);
	task->movingCharacter = MovingCharacter;
	task->targetLocation = TargetLocation;
	task->moveSpeed = MoveSpeed;
	task->cachedMoveComp = MovingCharacter->GetCharacterMovement();

	if (ShouldAdjustHight)
	{
		// Add the Z value between our feet and real location so that we don't move into the ground.
		const float footLocationZ = task->cachedMoveComp->GetActorFeetLocation().Z;
		const float actorLocationZ = MovingCharacter->GetActorLocation().Z;
		const float targetOffsetZ = actorLocationZ - footLocationZ;
		task->targetLocation.Z += targetOffsetZ;
	}

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
	cachedMoveComp->StopMovementImmediately();
	cachedMoveComp->SetMovementMode(MOVE_Flying);
	movingCharacter->SetAbilityMoveTargetLocation(targetLocation);
}

void UAbilityTask_MoveTo::TickTask(float DeltaTime)
{
	Super::TickTask(DeltaTime);

	const FVector actorLocation = movingCharacter->GetActorLocation();
	const FVector distanceVec = targetLocation - actorLocation;
	const float distance = distanceVec.Size();

	const float moveDeltaDistance = moveSpeed * DeltaTime;

	// Offset needed, at least in non-vr. Maybe we can get rid of it or make it smaller in VR.
	if (distance < moveDeltaDistance + 30.f)
	{
		movingCharacter->SetActorLocation(targetLocation);
		EndTask();
		OnLocationReached.Broadcast();
	}
	else
	{
 		const FVector direction = distanceVec / distance;
		const FVector Velocity = direction * moveSpeed;
		cachedMoveComp->MoveSmooth(Velocity, DeltaTime);
	}
}


void UAbilityTask_MoveTo::OnDestroy(bool AbilityEnded)
{
	Super::OnDestroy(AbilityEnded);

	cachedMoveComp->SetMovementMode(MOVE_Falling);
	cachedMoveComp->StopMovementImmediately();
	movingCharacter->InvalidateAbilityMoveTargetLocation();
}

void UAbilityTask_MoveTo::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UAbilityTask_MoveTo, movingCharacter);
	DOREPLIFETIME(UAbilityTask_MoveTo, targetLocation);
	DOREPLIFETIME(UAbilityTask_MoveTo, moveSpeed);
	DOREPLIFETIME(UAbilityTask_MoveTo, cachedMoveComp);
}

