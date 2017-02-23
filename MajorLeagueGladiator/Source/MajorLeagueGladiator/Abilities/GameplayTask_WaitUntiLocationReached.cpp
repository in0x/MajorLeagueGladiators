#pragma once
#include "MajorLeagueGladiator.h"
#include "GameplayTask_WaitUntiLocationReached.h"
#include "Abilities/GameplayAbility.h"

UGameplayTask_WaitUntiLocationReached::UGameplayTask_WaitUntiLocationReached(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	bTickingTask = true;
}

UGameplayTask_WaitUntiLocationReached* UGameplayTask_WaitUntiLocationReached::Create(UGameplayAbility* ThisAbility, FName TaskName, const ACharacter* MovingCharacter)
{
	UGameplayTask_WaitUntiLocationReached* task = NewTask<UGameplayTask_WaitUntiLocationReached>(ThisAbility, TaskName);
	task->movingCharacter = MovingCharacter;
	return task;
}

void UGameplayTask_WaitUntiLocationReached::Activate()
{
	Super::Activate();
}

void UGameplayTask_WaitUntiLocationReached::TickTask(float DeltaTime)
{
	Super::TickTask(DeltaTime);

	if (movingCharacter->GetController()->IsFollowingAPath())
	{
		return;
	}
	else
	{
		OnLocationReached.Broadcast();
	}
}
