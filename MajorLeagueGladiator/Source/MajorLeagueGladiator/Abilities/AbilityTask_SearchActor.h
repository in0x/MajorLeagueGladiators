// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameplayAbilityTargetActor.h"
#include "AbilityTask_SearchActor.generated.h"

UCLASS()
class MAJORLEAGUEGLADIATOR_API AAbilityTask_SearchActor : public AGameplayAbilityTargetActor
{
	GENERATED_BODY()
	
public:	
	AAbilityTask_SearchActor();

	virtual void Tick(float DeltaSeconds) override;

	virtual bool IsConfirmTargetingAllowed() override;

	UPROPERTY()
	TWeakObjectPtr<USceneComponent> TargetingSceneComponent;

	UPROPERTY()
	TArray<TWeakObjectPtr<AActor>> IgnoredActors;

	UPROPERTY()
	float MaxRange;

private:

	bool IsPullable(const AActor& Actor) const;
	
	static FGameplayAbilityTargetDataHandle MakeDataHandle(const FHitResult& HitResult);
};
