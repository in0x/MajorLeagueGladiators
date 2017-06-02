// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Components/ActorComponent.h"
#include "WaveSystemComponent.generated.h"

DECLARE_MULTICAST_DELEGATE_TwoParams(RemainingEnemiesForWaveChangedDelegate, int32, int32);
DECLARE_MULTICAST_DELEGATE_OneParam(WaveDelegate, int32)

UENUM(BlueprintType)
namespace EWaveState
{
	enum Type
	{
		NotStarted,
		DuringWave,
		BetweemWave,
	};
}

struct WaveSystemSavedState;

/*
Manages the Enemy wave timings, such as when to start a new Wave, what wave number it should be and
when a wave is over. To Spawn a wave it makes a request to WaveSpawnerManagerComponent. Every enemy
that belongs to wave needs to call the OnEnemyKilly Method to notify this class of its death.
Holds the the current wave number, as well as the number of enemies that need to be defeated.
Provides delegates to notify others when the number of enemies changes, when a wave starts an a wave ends
*/
UCLASS()
class MAJORLEAGUEGLADIATOR_API UWaveSystemComponent : public UActorComponent
{
	GENERATED_BODY()
public:
	UWaveSystemComponent();

	void OnEnemyKilled(ACharacter* KilledCharacter);

	void SetStartWave(int32 WaveNumber);
	void StartWave();
	void Stop();

	bool IsRunning() const;
	int32 GetRemainingEnemiesForWave() const;
	int32 GetCurrentWaveNumber() const;

	virtual void InitializeComponent() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	RemainingEnemiesForWaveChangedDelegate OnRemainingEnemiesForWaveChanged;
	WaveDelegate OnWaveStarted;
	WaveDelegate OnWaveCleared;

private:
	void SetFromSavedState(const WaveSystemSavedState& savedState);
	void WriteIntoSavedState(WaveSystemSavedState& savedState) const;

	void startNextWave();
	void startWaveImpl(int32 WaveNumber);
	void changeRemainingEnemiesForWave(int32 ChangeInValue);
	void setRemainingEnemiesForWave(int32 NewRemainingEnemiesForWave);
	void setWaveNumber(int32 NewWaveNumber);

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UFUNCTION()
	void onRep_remainingEnemiesForWave(int32 oldRemainingEnemiesForWave);

	UFUNCTION()
	void onRep_currentWaveNumber(int32 oldWaveNumber);

	void fireRemainingEnemiesForWaveChangedDelegates(int32 oldRemainingEnemiesForWave);
	void fireWaveNumberChangedDelegates(int32 oldWaveNumber);

	void playFirstWaveEffects();
	void playBeginOfWaveEffects();
	void playEndOfWaveEffects();

	UPROPERTY(ReplicatedUsing = onRep_remainingEnemiesForWave, Transient)
	int32 remainingEnemiesForWave;

	UPROPERTY(Replicated)
	TEnumAsByte<EWaveState::Type> waveState;

	UPROPERTY(EditAnywhere)
	int32 startWaveNumber;

	UPROPERTY(EditAnywhere)
	float initialTimeBeforeStartSeconds;

	UPROPERTY(EditAnywhere)
	float timeBetweenWavesSeconds;

	UPROPERTY(ReplicatedUsing = onRep_currentWaveNumber, Transient)
	int32 currentWaveNumber;

	UPROPERTY(EditAnywhere)
	USoundBase* firstWaveSound;

	UPROPERTY(EditAnywhere)
	USoundBase* beginOfWaveSound;

	UPROPERTY(EditAnywhere)
	USoundBase* endWaveSound;

	FTimerHandle nextActionTimerHandle;
};

