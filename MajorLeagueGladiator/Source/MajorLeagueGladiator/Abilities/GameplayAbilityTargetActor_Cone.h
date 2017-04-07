#pragma once
// Fill out your copyright notice in the Description page of Project Settings.

#include "Abilities/GameplayAbilityTargetActor.h"
#include "GameplayAbilityTargetActor_Cone.generated.h"

class USceneComponent;

UCLASS()
class MAJORLEAGUEGLADIATOR_API AGameplayAbilityTargetActor_Cone : public AGameplayAbilityTargetActor
{
	GENERATED_BODY()
public:

	float Distance;
	float HalfAngleDegrees;

	UPROPERTY(Transient)
	TArray<TWeakObjectPtr<AActor>> IgnoredActors;

	virtual void ConfirmTargetingAndContinue() override;

private:
	TArray<TWeakObjectPtr<AActor>> Trace() const;
	FGameplayAbilityTargetDataHandle MakeTargetHandle(const TArray<TWeakObjectPtr<AActor>> Actors) const;
};
