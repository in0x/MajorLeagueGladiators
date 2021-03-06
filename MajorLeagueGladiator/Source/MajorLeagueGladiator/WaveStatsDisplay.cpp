// Fill out your copyright notice in the Description page of Project Settings.

#include "MajorLeagueGladiator.h"
#include "WaveStatsDisplay.h"

#include "WaveSystem/WaveSystemComponent.h"
#include "Components/WidgetComponent.h"

// Sets default values
AWaveStatsDisplay::AWaveStatsDisplay(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{}

// Called when the game starts or when spawned
void AWaveStatsDisplay::BeginPlay()
{
	Super::BeginPlay();
	AGameStateBase* gamestate = GetWorld()->GetGameState();
	UWaveSystemComponent* waveSystemComponent = gamestate->FindComponentByClass<UWaveSystemComponent>();
	waveSystemComponent->OnRemainingEnemiesForWaveChanged.AddUObject(this, &AWaveStatsDisplay::OnEnemyCountChanged);
	waveSystemComponent->OnWaveNumberChanged.AddUObject(this, &AWaveStatsDisplay::OnWaveNumberChanged);

	OnEnemyCountChanged(waveSystemComponent->GetRemainingEnemiesForWave(), -1);
	OnWaveNumberChanged(waveSystemComponent->GetCurrentWaveNumber(), -1);
}

void AWaveStatsDisplay::OnWaveNumberChanged(int NewWaveCount, int OldWaveNumber)
{
	UpdateWaveNumber(NewWaveCount);
}
void AWaveStatsDisplay::OnEnemyCountChanged(int NewCount, int OldCount)
{
	UpdateEnemyCount(NewCount);
}

void AWaveStatsDisplay::UpdateWaveNumber_Implementation(int NewWaveCount){}

void AWaveStatsDisplay::UpdateEnemyCount_Implementation(int NewEnemyCount){}


