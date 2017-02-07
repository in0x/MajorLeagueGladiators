// Fill out your copyright notice in the Description page of Project Settings.

#include "MajorLeagueGladiator.h"
#include "AbilityTask_PullTarget.h"

#include "AbilityTask_PullTargetActor.h"

UAbilityTask_PullTarget* UAbilityTask_PullTarget::Create(UObject* WorldContextObject, FName TaskName, AActor* TargetActor, USceneComponent* EndLocation, float PullSpeed, float MinDistanceThreshold)
{
	UAbilityTask_PullTarget* result = NewAbilityTask<UAbilityTask_PullTarget>(WorldContextObject, TaskName);

	UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject);
	AAbilityTask_PullTargetActor* SpawnedActor = World->SpawnActorDeferred<AAbilityTask_PullTargetActor>(AAbilityTask_PullTargetActor::StaticClass(), FTransform::Identity, nullptr, nullptr, ESpawnActorCollisionHandlingMethod::AlwaysSpawn);

	SpawnedActor->PullSpeed = PullSpeed;
	SpawnedActor->EndLocationSceneComponent = EndLocation;	
	SpawnedActor->TargetRootComponent = TargetActor->GetRootComponent();
	SpawnedActor->MinDistanceThreshold = MinDistanceThreshold;

	SpawnedActor->OnLocationReached.BindUObject(result, &UAbilityTask_PullTarget::OnLocationReachedCallback);

	result->spawnedActor = SpawnedActor;
	result->targetActor = TargetActor;

	return result;
}


void UAbilityTask_PullTarget::Activate()
{
	spawnedActor->FinishSpawning(FTransform::Identity);
	UPrimitiveComponent* rootComp = Cast<UPrimitiveComponent>(targetActor->GetRootComponent());
	if (rootComp)
	{
		rootComp->SetEnableGravity(false);
	}
}

void UAbilityTask_PullTarget::OnLocationReachedCallback()
{
	spawnedActor->Destroy();
	check(targetActor.IsValid());
	UPrimitiveComponent* rootComp = Cast<UPrimitiveComponent>(targetActor->GetRootComponent());
	if (rootComp)
	{
		rootComp->SetEnableGravity(true);
	}	

	OnSuccess.Broadcast(targetActor.Get());

	EndTask();
}

void UAbilityTask_PullTarget::OnDestroy(bool AbilityEnded)
{
	if (spawnedActor)
	{
		spawnedActor->Destroy();
	}

	Super::OnDestroy(AbilityEnded);
}