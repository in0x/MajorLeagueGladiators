// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Components/ActorComponent.h"
#include "WaveSystemComponent.generated.h"

DECLARE_MULTICAST_DELEGATE_TwoParams(EnemyCountChangedDelegate, int32, int32);
DECLARE_MULTICAST_DELEGATE(EnemyCountZeroDelegate);

UCLASS()
class MAJORLEAGUEGLADIATOR_API UWaveSystemComponent : public UActorComponent
{
	GENERATED_BODY()
public:
	UWaveSystemComponent();
	void OnEnemyKilled(ACharacter* KilledCharacter);

	void StartWave(int32 WaveNumber);

	void ChangeEnemyCount(int32 ChangeInValue);
	void SetEnemyCount(int32 NewEnemyCount);
	int32 GetEnemyCount() const;

	EnemyCountChangedDelegate OnEnemyCountChanged;
	EnemyCountZeroDelegate OnEnemyCountZero;
private:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UFUNCTION()
	void onRep_EnemyCount(int32 oldEnemyCount);

	UPROPERTY(ReplicatedUsing = onRep_EnemyCount, Transient)
	int32 enemyCount;

	UPROPERTY(EditAnywhere)
	int32 startWaveNumber;

	int32 currentWaveNumber;
};

