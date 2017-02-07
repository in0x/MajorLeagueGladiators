// Fill out your copyright notice in the Description page of Project Settings.

#include "MajorLeagueGladiator.h"
#include "AbilityTask_Search.h"

#include "VRExpansion/VRGripInterface.h"
#include "AbilityTask_SearchActor.h"

UAbilityTask_Search::UAbilityTask_Search(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)

{
	bTickingTask = true;
}

UAbilityTask_Search* UAbilityTask_Search::Create(UObject* WorldContextObject, FName Name)
{
	UAbilityTask_Search* result = NewAbilityTask<UAbilityTask_Search>(WorldContextObject, Name);

	result->searchActorClass = AAbilityTask_SearchActor::StaticClass();
	return result;
}

bool UAbilityTask_Search::BeginSpawningActor(UObject* WorldContextObject, TSubclassOf<AAbilityTask_SearchActor> InTargetClass, AAbilityTask_SearchActor*& SpawnedActor)
{
	SpawnedActor = nullptr;

	if (Ability)
	{
		UClass* Class = *InTargetClass;

		if (Class != nullptr)
		{
			UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject);
			SpawnedActor = World->SpawnActorDeferred<AAbilityTask_SearchActor>(Class, FTransform::Identity, nullptr, nullptr, ESpawnActorCollisionHandlingMethod::AlwaysSpawn);
		}

		if (SpawnedActor)
		{
			searchActor = SpawnedActor;
			searchActor->OnActorFound.BindUObject(this, &UAbilityTask_Search::ActorReadyCallback);
		}
	}
	return (SpawnedActor != nullptr);
}

void UAbilityTask_Search::FinishSpawningActor(UObject* WorldContextObject, AAbilityTask_SearchActor* SpawnedActor)
{
	if (SpawnedActor && !SpawnedActor->IsPendingKill())
	{
		check(searchActor == SpawnedActor);

		const FTransform SpawnTransform = AbilitySystemComponent->GetOwner()->GetTransform();

		SpawnedActor->FinishSpawning(SpawnTransform);
	}
}

void UAbilityTask_Search::ActorReadyCallback(AActor* Actor)
{
	OnSuccess.Broadcast(Actor);
	EndTask();
}


void UAbilityTask_Search::OnDestroy(bool AbilityEnded)
{
	if (searchActor)
	{
		searchActor->Destroy();

	}
	Super::OnDestroy(AbilityEnded);
}