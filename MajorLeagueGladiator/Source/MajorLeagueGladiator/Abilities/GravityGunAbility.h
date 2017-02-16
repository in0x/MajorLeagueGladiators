#pragma once
// Fill out your copyright notice in the Description page of Project Settings.

#include "Abilities/GameplayAbility.h"
#include "GravityGunAbility.generated.h"

class UVRControllerComponent;
class UAbilityTask_Search;
class UAbilityTask_WaitTargetData;
class UAbilityTask_PullTarget;

UCLASS()
class MAJORLEAGUEGLADIATOR_API UGravityGunAbility : public UGameplayAbility
{
	GENERATED_BODY()
public:
	UGravityGunAbility(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	// Max Range you can pull from
	UPROPERTY(EditAnywhere)
	float PullRange;

	// How fast the Object gets pulled
	UPROPERTY(EditAnywhere)
	float PullSpeed;

	// If the pulled Object reaches that Range you grab it
	UPROPERTY(EditAnywhere)
	float GrabRange;

	// How fast the pulled Object gets launched
	UPROPERTY(EditAnywhere)
	float LaunchVelocity;

private:
	virtual void InputPressed(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo) override;
	virtual void InputReleased(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo) override;
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

	void SetGripControllerFromOwner();

	void SearchAndPull();
	void LaunchGrippedActor();

	UFUNCTION()
	void OnSearchSuccessful(const FGameplayAbilityTargetDataHandle& Data);

	UFUNCTION()
	void OnSearchCancelled(const FGameplayAbilityTargetDataHandle& Data);

	void OnActorPullFinished(AActor* pulledActor);
	void OnActorPullFailed();

	bool HasGrippedActor() const;

	UPROPERTY(Transient)
	UAbilityTask_PullTarget* pullTask;
	
	UPROPERTY(Transient)
	UAbilityTask_WaitTargetData* searchTask;

	UPROPERTY(Transient)
	UVRControllerComponent* gripController;

	UPROPERTY(Transient)
	UMeshComponent* gripControllerMesh;
};
