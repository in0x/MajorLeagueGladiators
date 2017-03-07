// Fill out your copyright notice in the Description page of Project Settings.

#include "MajorLeagueGladiator.h"
#include "AbilityTask_MoveTo.h"
#include "AbilityTask_MoveToActor.h"
#include "MlgPlayerCharacter.h"

UAbilityTask_MoveTo* UAbilityTask_MoveTo::Create(UGameplayAbility* ThisAbility, FName TaskName, FVector TargetLocation, float MoveSpeed, AMlgPlayerCharacter* MovingCharacter)
{
	UAbilityTask_MoveTo* task = NewAbilityTask<UAbilityTask_MoveTo>(ThisAbility, TaskName);
	auto world = GEngine->GetWorldFromContextObject(ThisAbility);
	auto SpawnedActor = world->SpawnActorDeferred<AAbilityTask_MoveToActor>(AAbilityTask_MoveToActor::StaticClass(), FTransform::Identity, nullptr, nullptr, ESpawnActorCollisionHandlingMethod::AlwaysSpawn);

	SpawnedActor->TargetLocation = TargetLocation;
	SpawnedActor->MoveSpeed = MoveSpeed;
	SpawnedActor->MovingCharacter = MovingCharacter;

	task->spawnedActor = SpawnedActor;

	return task;
}

void UAbilityTask_MoveTo::Activate()
{
	Super::Activate();

	spawnedActor->FinishSpawning(FTransform::Identity);	
	spawnedActor->OnLocationReached.AddUObject(this, &UAbilityTask_MoveTo::OnLocationReachedCallback);
}

void UAbilityTask_MoveTo::OnLocationReachedCallback()
{
	EndTask();
	OnLocationReached.Broadcast();
}

void UAbilityTask_MoveTo::OnDestroy(bool AbilityEnded)
{
	if (spawnedActor)
	{
		spawnedActor->Destroy();
	}

	Super::OnDestroy(AbilityEnded);
}
