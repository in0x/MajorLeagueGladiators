// Fill out your copyright notice in the Description page of Project Settings.

#include "MajorLeagueGladiator.h"
#include "AbilityTask_DashTo.h"

UAbilityTask_DashTo::UAbilityTask_DashTo()
{
	bTickingTask = true;
	bSimulatedTask = true;
}

UAbilityTask_DashTo* UAbilityTask_DashTo::Create(UGameplayAbility* ThisAbility, FName TaskName, const FVector& Velocity, float TimeInSeconds)
{
	UAbilityTask_DashTo* createdTask = NewAbilityTask<UAbilityTask_DashTo>(ThisAbility, TaskName);

	createdTask->velocity = Velocity;
	createdTask->time = TimeInSeconds;

	return createdTask;
}

void UAbilityTask_DashTo::Activate()
{
	ACharacter* ownerActor = CastChecked<ACharacter>(GetAvatarActor());
	cachedPrimitive = CastChecked<UPrimitiveComponent>(ownerActor->GetRootComponent());

	cachedMovementComponent = CastChecked<UCharacterMovementComponent>(ownerActor->GetMovementComponent());
	check(cachedMovementComponent);

	cachedMovementComponent->StopMovementImmediately();
	
	cachedMovementComponent->SetMovementMode(MOVE_Custom, 0);
}

void UAbilityTask_DashTo::OnDestroy(bool bInOwnerFinished)
{
	Super::OnDestroy(bInOwnerFinished);
	if (cachedMovementComponent->MovementMode == MOVE_Custom)
	{
		cachedMovementComponent->SetMovementMode(MOVE_Falling);
	}
}


void UAbilityTask_DashTo::TickTask(float DeltaTime)
{
	FHitResult hitresult;

	FVector moveDelta = CalcMovementDelta(DeltaTime);
	FQuat rotation = cachedPrimitive->GetComponentQuat();

	cachedPrimitive->GetOwner()->SetActorLocation(cachedPrimitive->GetComponentLocation() + moveDelta, true, &hitresult);

	if (hitresult.bBlockingHit)
	{
		EndTask();
		OnCollision.Broadcast(hitresult);
	}
}

FVector UAbilityTask_DashTo::CalcMovementDelta(float DeltaTime) const
{
	return velocity * DeltaTime;
}