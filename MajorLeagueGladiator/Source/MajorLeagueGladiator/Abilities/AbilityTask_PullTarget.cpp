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
	SetActorGravity_NetMulticast(targetActor.Get(), false);
	
}

void UAbilityTask_PullTarget::OnLocationReachedCallback()
{
	spawnedActor->Destroy();
	check(targetActor.IsValid())

	OnSuccess.Broadcast(targetActor.Get());

	EndTask();
}

void UAbilityTask_PullTarget::OnDestroy(bool AbilityEnded)
{
	SetActorGravity_NetMulticast(targetActor.Get(), true);
	if (spawnedActor)
	{
		spawnedActor->Destroy();
	}

	Super::OnDestroy(AbilityEnded);
}

void UAbilityTask_PullTarget::SetActorGravity_NetMulticast_Implementation(AActor* Actor, bool GravityEnabled)
{
	UPrimitiveComponent* rootComp = Cast<UPrimitiveComponent>(Actor->GetRootComponent());
	if (rootComp)
	{
		rootComp->SetEnableGravity(GravityEnabled);
	}
}
