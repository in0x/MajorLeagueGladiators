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
	virtual void InputPressed(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo) override;
	virtual void InputReleased(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo) override;
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;

private:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty> & OutLifetimeProps) const override;
	void pushAwayCloseActors();
	float calcSecondsToFullCharge() const;
	void onShieldChargeEmpty();

	// we might want to put this into a common base class for abilities that use motion controllers
	void SetGripControllerFromOwner();

	UFUNCTION()
	void OnBoxTraceFinished(const FGameplayAbilityTargetDataHandle& Data);

	void SpawnShield();
	void DespawnShield();

	UFUNCTION()
	void onRep_shieldActor();

	UPROPERTY(EditAnywhere)
	TSubclassOf<AShieldActor> shieldActorClass;

	UPROPERTY(EditAnywhere)
	FVector pushBoxHalfExtent;

	UPROPERTY(EditAnywhere)
	FVector pushBoxOffsetFromController;

	UPROPERTY(EditAnywhere)
	float pushVelocity;

	UPROPERTY(Transient)
	UMeshComponent* gripControllerMesh;

	UPROPERTY(Transient)
	UVRControllerComponent* gripController;

	UPROPERTY(Transient, ReplicatedUsing = onRep_shieldActor)
	AShieldActor* shieldActor;

	float timestampShieldFullyCharged;
};
