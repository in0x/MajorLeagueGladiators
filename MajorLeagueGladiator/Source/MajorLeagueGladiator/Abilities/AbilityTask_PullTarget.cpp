// Fill out your copyright notice in the Description page of Project Settings.

#include "MajorLeagueGladiator.h"
#include "AbilityTask_PullTarget.h"

#include "PackMovementComponent.h"

UAbilityTask_PullTarget* UAbilityTask_PullTarget::Create(UGameplayAbility* ThisAbility, FName TaskName, AActor* TargetActor, USceneComponent* EndLocation,
	float PullSpeed, float MaxDistanceThreshold)
{
	UAbilityTask_PullTarget* task = NewAbilityTask<UAbilityTask_PullTarget>(ThisAbility, TaskName);

	task->pullSpeed = PullSpeed;
	task->endLocationSceneComponent = EndLocation;
	task->targetActor = TargetActor;
	task->targetPrimitve = CastChecked<UPrimitiveComponent>(TargetActor->GetRootComponent());
	task->maxDistanceThreshold = MaxDistanceThreshold;

	return task;
}

UAbilityTask_PullTarget::UAbilityTask_PullTarget()
	: targetActor(nullptr)
	, targetPrimitve(nullptr)
	, endLocationSceneComponent(nullptr)
{
	bTickingTask = true;
	bSimulatedTask = true;
}

void UAbilityTask_PullTarget::TickTask(float DeltaTime)
{
	Super::TickTask(DeltaTime);
	const FVector targetLocation = targetPrimitve->GetComponentLocation();
	const FVector endLocation = endLocationSceneComponent->GetComponentLocation();

	const FVector distanceVec = endLocation - targetLocation;
	const float distance = distanceVec.Size();

	auto* moveComp = targetActor->FindComponentByClass<UPackMovementComponent>();
	check(moveComp)

	// Offset needed, at least in non-vr. Maybe we can get rid of it or make it smaller in VR.
	if (distance < pullSpeed * DeltaTime + 30.f)
	{
		moveComp->StopMovementImmediately();
		targetPrimitve->SetWorldLocation(endLocation);

		if (HasAuthority())
		{
			OnSuccess.Broadcast(targetActor);
		}
	}
	else if (distance > maxDistanceThreshold)
	{
		if (HasAuthority())
		{
			OnFail.Broadcast();
		}
	}
	else
	{
		const FVector dirVector = distanceVec / distance;

		moveComp->SetVelocity(dirVector * pullSpeed);
		DrawDebugDirectionalArrow(targetActor->GetWorld(), targetLocation, endLocation, 1.0f, FColor::Green);
	}
}



void UAbilityTask_PullTarget::Activate()
{
	if (!targetActor)
	{
		EndTask();
		OnFail.Broadcast();
		return;
	}

	targetActor->OnDestroyed.AddDynamic(this, &UAbilityTask_PullTarget::OnPulledActorDestroyed);

	SetActorGravity_NetMulticast(targetActor, false);

	targetPrimitve->MoveIgnoreActors.Add(endLocationSceneComponent->GetOwner());
}

void UAbilityTask_PullTarget::ExternalCancel()
{
	Super::ExternalCancel();
	OnFail.Broadcast();
}

void UAbilityTask_PullTarget::OnDestroy(bool AbilityEnded)
{
	if (!targetActor->IsPendingKill())
	{
		SetActorGravity_NetMulticast(targetActor, true);
		UPrimitiveComponent* rootComponent = CastChecked<UPrimitiveComponent>(targetActor->GetRootComponent());
		rootComponent->ClearMoveIgnoreActors();
	}	
	
	Super::OnDestroy(AbilityEnded);
}

void UAbilityTask_PullTarget::SetActorGravity_NetMulticast_Implementation(AActor* Actor, bool GravityEnabled)
{
	UPrimitiveComponent* rootComponent = CastChecked<UPrimitiveComponent>(Actor->GetRootComponent());
	rootComponent->SetEnableGravity(GravityEnabled);
}

void UAbilityTask_PullTarget::OnPulledActorDestroyed(AActor* DestroyedActor)
{
	check(targetActor == DestroyedActor);
	EndTask();
	OnFail.Broadcast();
}

bool UAbilityTask_PullTarget::HasAuthority() const
{
	return Ability &&  Ability->HasAuthority(&Ability->GetCurrentActivationInfoRef());
}
