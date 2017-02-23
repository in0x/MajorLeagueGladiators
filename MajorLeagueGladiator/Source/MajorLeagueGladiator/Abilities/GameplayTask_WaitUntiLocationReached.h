// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameplayTask.h"
#include "GameplayTask_WaitUntiLocationReached.generated.h"

DECLARE_MULTICAST_DELEGATE(FGameplayTaskWaitLocationReachedDelegate);

class UGameplayAbility;

UCLASS()
class MAJORLEAGUEGLADIATOR_API UGameplayTask_WaitUntiLocationReached : public UGameplayTask
{
	GENERATED_BODY()

public:
	UGameplayTask_WaitUntiLocationReached(const FObjectInitializer& ObjectInitializer);

	static UGameplayTask_WaitUntiLocationReached* Create(UGameplayAbility* ThisAbility, FName TaskName, const ACharacter* MovingCharacter);

	virtual void Activate() override;
	virtual void TickTask(float DeltaTime) override; 

	FGameplayTaskWaitLocationReachedDelegate OnLocationReached;
	
private:
	UPROPERTY()
	TWeakObjectPtr<ACharacter> movingCharacter;
};
