// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Abilities/Tasks/AbilityTask.h"

#include "AbilityTask_Search.generated.h"


DECLARE_MULTICAST_DELEGATE_OneParam(FOnSuccessDelegate, AActor*);

class AAbilityTask_SearchActor;

UCLASS()
class MAJORLEAGUEGLADIATOR_API UAbilityTask_Search : public UAbilityTask
{
	GENERATED_BODY()	

public:
	UAbilityTask_Search(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	static UAbilityTask_Search* Create(UObject* WorldContextObject, FName Name);

	bool BeginSpawningActor(UObject* WorldContextObject, TSubclassOf<AAbilityTask_SearchActor> InTargetClass, AAbilityTask_SearchActor*& SpawnedActor);
	void FinishSpawningActor(UObject* WorldContextObject, AAbilityTask_SearchActor* SpawnedActor);

	virtual void OnDestroy(bool AbilityEnded) override;

	FOnSuccessDelegate OnSuccess;
private:

	UFUNCTION()
	void ActorReadyCallback(AActor* Actor);

	UPROPERTY()
	TSubclassOf<AAbilityTask_SearchActor> searchActorClass;

	UPROPERTY()
	AAbilityTask_SearchActor* searchActor;
};
