// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Components/ActorComponent.h"
#include "WaveSystemComponent.generated.h"

DECLARE_MULTICAST_DELEGATE_TwoParams(RemainingEnemiesForWaveChangedDelegate, int32, int32);
DECLARE_MULTICAST_DELEGATE_OneParam(WaveDelegate, int32)

UCLASS()
class MAJORLEAGUEGLADIATOR_API UWaveSystemComponent : public UActorComponent
{
	GENERATED_BODY()
public:
	UWaveSystemComponent();

	virtual void BeginPlay() override;
	void OnEnemyKilled(ACharacter* KilledCharacter);

	void Start();
	void StartNextWave();
	void StartWave(int32 WaveNumber);

	void ChangeRemainingEnemiesForWave(int32 ChangeInValue);
	void SetRemainingEnemiesForWave(int32 NewRemainingEnemiesForWave);
	int32 GetRemainingEnemiesForWave() const;

	RemainingEnemiesForWaveChangedDelegate OnRemainingEnemiesForWaveChanged;
	WaveDelegate OnWaveStarted;
	WaveDelegate OnWaveCleared;
private:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UFUNCTION()
	void onRep_remainingEnemiesForWave(int32 oldRemainingEnemiesForWave);
	void fireRemainingEnemiesForWaveChangedDelegates(int32 oldRemainingEnemiesForWave);

	UPROPERTY(ReplicatedUsing = onRep_remainingEnemiesForWave, Transient)
	int32 remainingEnemiesForWave;

	UPROPERTY(EditAnywhere)
	int32 startWaveNumber;

	UPROPERTY(EditAnywhere)
	float initialTimeBeforeStartSeconds;

	UPROPERTY(EditAnywhere)
	float timeBetweenWavesSeconds;

	int32 currentWaveNumber;
};

