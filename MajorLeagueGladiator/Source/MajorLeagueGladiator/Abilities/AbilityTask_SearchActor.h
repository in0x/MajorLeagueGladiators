// Fill out your copyright notice in the Description page of Project Settings.

#pragma once


#include "GameplayAbilityTargetActor.h"


#include "AbilityTask_SearchActor.generated.h"

DECLARE_DELEGATE_OneParam(FOnActorFoundDelegate, AActor*);

UCLASS()
class MAJORLEAGUEGLADIATOR_API AAbilityTask_SearchActor : public AGameplayAbilityTargetActor
{
	GENERATED_BODY()
	
public:	
	AAbilityTask_SearchActor();

	virtual void BeginPlay() override;
	
	virtual void Tick(float DeltaSeconds) override;

	virtual bool IsConfirmTargetingAllowed() override;

	FOnActorFoundDelegate OnActorFound;

	UPROPERTY()
	TWeakObjectPtr<USceneComponent> TargetingSceneComponent;

	UPROPERTY()
	TArray<TWeakObjectPtr<AActor>> IgnoredActors;

	UPROPERTY()
	float maxRange;

private:

	bool IsValidActor(const AActor& Actor) const;
	
	FGameplayAbilityTargetDataHandle MakeDataHandle(const FHitResult& HitResult);
};
