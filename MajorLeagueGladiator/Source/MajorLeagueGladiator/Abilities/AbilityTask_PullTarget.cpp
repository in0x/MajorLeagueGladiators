// Fill out your copyright notice in the Description page of Project Settings.

#include "MajorLeagueGladiator.h"
#include "AbilityTask_PullTarget.h"

#include "AbilityTask_PullTargetActor.h"

UAbilityTask_PullTarget* UAbilityTask_PullTarget::Create(UGameplayAbility* ThisAbility, FName TaskName, AActor* TargetActor, USceneComponent* EndLocation, float PullSpeed, float MinDistanceThreshold)
{
	UAbilityTask_PullTarget* task = NewAbilityTask<UAbilityTask_PullTarget>(ThisAbility, TaskName);

	UWorld* World = GEngine->GetWorldFromContextObject(ThisAbility);
	AAbilityTask_PullTargetActor* SpawnedActor = World->SpawnActorDeferred<AAbilityTask_PullTargetActor>(AAbilityTask_PullTargetActor::StaticClass(),
		FTransform::Identity, nullptr, nullptr, ESpawnActorCollisionHandlingMethod::AlwaysSpawn);

	SpawnedActor->PullSpeed = PullSpeed;
	SpawnedActor->EndLocationSceneComponent = EndLocation;	
	SpawnedActor->TargetRootComponent = CastChecked<UPrimitiveComponent>(TargetActor->GetRootComponent());
	SpawnedActor->MinDistanceThreshold = MinDistanceThreshold;

	SpawnedActor->OnLocationReached.BindUObject(task, &UAbilityTask_PullTarget::OnLocationReachedCallback);
	SpawnedActor->OnFail.BindUObject(task, &UAbilityTask_PullTarget::OnFailedCallback);

	task->spawnedActor = SpawnedActor;
	task->targetActor = TargetActor;

	return task;
}


void UAbilityTask_PullTarget::Activate()
{
	if (targetActor.IsStale() || !targetActor.IsValid())
	{
		EndTask();
		OnFail.Broadcast();
		return;
	}

	targetActor->OnDestroyed.AddDynamic(this, &UAbilityTask_PullTarget::OnPulledActorDestroyed);

	spawnedActor->FinishSpawning(FTransform::Identity);
	SetActorGravity_NetMulticast(targetActor.Get(), false);

	UPrimitiveComponent* rootComponent = CastChecked<UPrimitiveComponent>(targetActor->GetRootComponent());	
	rootComponent->MoveIgnoreActors.Add(spawnedActor->EndLocationSceneComponent->GetOwner());	
}

void UAbilityTask_PullTarget::ExternalCancel()
{
	Super::ExternalCancel();
	OnFail.Broadcast();
}

void UAbilityTask_PullTarget::OnLocationReachedCallback()
{
	check(targetActor.IsValid());

	EndTask();
	OnSuccess.Broadcast(targetActor.Get());	
}

void UAbilityTask_PullTarget::OnFailedCallback()
{
	EndTask();
	OnFail.Broadcast();
}

void UAbilityTask_PullTarget::OnDestroy(bool AbilityEnded)
{
	if (spawnedActor)
	{
		spawnedActor->Destroy();
	}
	if (targetActor.IsValid() && !targetActor.IsStale() && !targetActor->IsPendingKill())
	{
		SetActorGravity_NetMulticast(targetActor.Get(), true);
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