// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

DECLARE_MULTICAST_DELEGATE_OneParam(FAbilityTaskPullTargetSuccessDelegate, AActor*);
DECLARE_MULTICAST_DELEGATE(FAbilityTaskPullTargetFailDelegate);

#include "Abilities/Tasks/AbilityTask.h"
#include "AbilityTask_PullTarget.generated.h"

class AAbilityTask_PullTargetActor;

UCLASS()
class MAJORLEAGUEGLADIATOR_API UAbilityTask_PullTarget : public UAbilityTask
{
	GENERATED_BODY()
	
public:
	static UAbilityTask_PullTarget* Create(UGameplayAbility* ThisAbility, FName TaskName, AActor* TargetActor, USceneComponent* EndLocation,
		float PullSpeed, float MinDistanceThreshold, float MaxDistanceThreshold = 2000.f);

	UAbilityTask_PullTarget();

	virtual void Activate() override;
	virtual void ExternalCancel() override;
	virtual void TickTask(float DeltaTime) override;
	void OnDestroy(bool AbilityEnded) override;

	FAbilityTaskPullTargetSuccessDelegate OnSuccess;
	FAbilityTaskPullTargetFailDelegate OnFail;

	UFUNCTION(reliable, NetMulticast)
	void SetActorGravity_NetMulticast(AActor* Actor, bool GravityEnabled);
private:

	UPROPERTY(Transient)
	AActor* targetActor;

	UPROPERTY(Transient)
	UPrimitiveComponent* targetPrimitve;

	UPROPERTY(Transient)
	USceneComponent* endLocationSceneComponent;

	float pullSpeed;
	float minDistanceThreshold;
	float maxDistanceThreshold;

	UFUNCTION()
	void OnPulledActorDestroyed(AActor* DestroyedActor);

	bool HasAuthority() const;
};
