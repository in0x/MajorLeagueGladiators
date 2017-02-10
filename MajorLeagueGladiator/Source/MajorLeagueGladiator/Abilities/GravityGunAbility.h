#pragma once
// Fill out your copyright notice in the Description page of Project Settings.

#include "Abilities/GameplayAbility.h"
#include "GravityGunAbility.generated.h"

class UVRControllerComponent;
class UAbilityTask_Search;

UCLASS()
class MAJORLEAGUEGLADIATOR_API UGravityGunAbility : public UGameplayAbility
{
	GENERATED_BODY()
public:
	UGravityGunAbility(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
private:
	virtual void InputReleased(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo) override;
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

	void SetGripControllerFromOwner();

	void SearchAndPull();
	void LaunchGrippedActor();

	UFUNCTION()
	void OnSearchSuccessful(const FGameplayAbilityTargetDataHandle& Data);

	void OnActorPullFinished(AActor* pulledActor);

	bool HasGrippedActor() const;

	UPROPERTY()
	TWeakObjectPtr<UAbilityTask> currentTask;

	UPROPERTY()
	UVRControllerComponent* gripController;


};
