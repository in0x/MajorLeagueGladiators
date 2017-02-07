// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

DECLARE_MULTICAST_DELEGATE_OneParam(FAbilityTaskPullTargetSuccessDelegate, AActor*);

#include "Abilities/Tasks/AbilityTask.h"
#include "AbilityTask_PullTarget.generated.h"

class AAbilityTask_PullTargetActor;

UCLASS()
class MAJORLEAGUEGLADIATOR_API UAbilityTask_PullTarget : public UAbilityTask
{
	GENERATED_BODY()
	
public:
	static UAbilityTask_PullTarget* Create(UObject* WorldContextObject, FName TaskName, AActor* TargetActor, USceneComponent* EndLocation, float PullSpeed, float MinDistanceThreshold);

	virtual void Activate() override;
	void OnDestroy(bool AbilityEnded);

	FAbilityTaskPullTargetSuccessDelegate OnSuccess;
private:
	AAbilityTask_PullTargetActor* spawnedActor;

	UPROPERTY()
	TWeakObjectPtr<AActor> targetActor;

	void OnLocationReachedCallback();
	
};
