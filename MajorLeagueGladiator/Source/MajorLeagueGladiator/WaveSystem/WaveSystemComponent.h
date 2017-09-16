// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Components/ActorComponent.h"
#include "WaveSystemComponent.generated.h"

DECLARE_MULTICAST_DELEGATE_TwoParams(RemainingEnemiesForWaveChangedDelegate, int32, int32);
DECLARE_MULTICAST_DELEGATE_TwoParams(WaveChangedDelegate, int32, int32)
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

// NOTE(Phil): UE4 doesnt seem to let me use bitflag enum values another way. Errors
// when combination is specified as OR (i.e. FirstBegin = FirstWave | BeginWave). 
// Probably due to macros.
UENUM(BlueprintType)
namespace EPlaySoundSettings
{
	enum Type
	{
		None = 0			UMETA(DisplayName = "None"),
		FirstWave = 1		UMETA(DisplayName = "FirstWave"),
		BeginWave = 2		UMETA(DisplayName = "BeginWave"),
		FirstBegin = 3		UMETA(DisplayName = "FirstBegin"),
		EndWave = 4			UMETA(DisplayName = "EndWave"), 
		FirstEnd = 5 		UMETA(DisplayName = "FirstEnd"),
		BeginEnd = 6		UMETA(DisplayName = "BeginEnd"),
		FirstBeginEnd = 7	UMETA(DisplayName = "FirstBeginEnd")
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
	virtual void UninitializeComponent() override;

	RemainingEnemiesForWaveChangedDelegate OnRemainingEnemiesForWaveChanged;
	WaveChangedDelegate OnWaveNumberChanged;
	WaveDelegate OnWaveStarted;
	WaveDelegate OnWaveCleared;

private:
	void setFromSavedState(const WaveSystemSavedState& savedState);
	void writeIntoSavedState(WaveSystemSavedState& savedState) const;

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
	bool HasSoundSetting(EPlaySoundSettings::Type option);

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

	UPROPERTY(EditAnywhere)
	TEnumAsByte<EPlaySoundSettings::Type> soundSettings;

	FTimerHandle startNextWaveTimerHandle;
};

