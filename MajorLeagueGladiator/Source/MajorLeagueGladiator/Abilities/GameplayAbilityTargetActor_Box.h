// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Abilities/GameplayAbilityTargetActor.h"
#include "GameplayAbilityTargetActor_Box.generated.h"

class USceneComponent;

UCLASS()
class MAJORLEAGUEGLADIATOR_API AGameplayAbilityTargetActor_Box : public AGameplayAbilityTargetActor
{
	GENERATED_BODY()
public:
	
	//Half length, half width, half height
	void SetHalfExtent(const FVector& HalfExtent);


	//Offset Relative to the position. Rotation influences this
	FVector LocationOffsetFromPositon;

	UPROPERTY()
	TArray<TWeakObjectPtr<AActor>> IgnoredActors;

	virtual void ConfirmTargetingAndContinue() override;
	
private:
	FVector CalcCenterBox() const;
	TArray<TWeakObjectPtr<AActor>> Trace() const;
	FGameplayAbilityTargetDataHandle MakeTargetHandle(const TArray<TWeakObjectPtr<AActor>> Actors) const;

	FCollisionShape boxCollsionShape;
};
