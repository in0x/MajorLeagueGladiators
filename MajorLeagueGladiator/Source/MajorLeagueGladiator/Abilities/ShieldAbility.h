// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Abilities/GameplayAbility.h"
#include "ShieldAbility.generated.h"

class UVRControllerComponent;
class AShieldActor;

UCLASS()
class MAJORLEAGUEGLADIATOR_API UShieldAbility : public UGameplayAbility
{
	GENERATED_BODY()
public:
	UShieldAbility();
	virtual void InputReleased(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo) override;
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility) override;


private:
	void PushAwayCloseActors();

	// we might want to put this into a common base class for abilities that use motion controllers
	void SetGripControllerFromOwner();

	UFUNCTION()
	void OnBoxTraceFinished(const FGameplayAbilityTargetDataHandle& Data);

	void SpawnShield();
	void DespawnShield();

	UPROPERTY(EditAnywhere)
	TSubclassOf<AShieldActor> shieldActorClass;

	UPROPERTY(Transient)
	UMeshComponent* gripControllerMesh;

	UPROPERTY(Transient)
	UVRControllerComponent* gripController;

	UPROPERTY(Transient)
	AActor* shieldActor;
};
