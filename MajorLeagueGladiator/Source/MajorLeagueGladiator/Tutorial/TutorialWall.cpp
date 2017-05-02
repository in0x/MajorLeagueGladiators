// Fill out your copyright notice in the Description page of Project Settings.

#include "MajorLeagueGladiator.h"
#include "TutorialWall.h"
#include "WaveSystem/WaveSystemComponent.h"

void ATutorialWall::BeginPlay()
{
	Super::BeginPlay();

	if (HasAuthority())
	{
		UWaveSystemComponent* waveSystem = GetWorld()->GetGameState()->FindComponentByClass<UWaveSystemComponent>();
		waveSystem->OnWaveCleared.AddUObject(this, &ATutorialWall::OnWaveCleared);
	}
}

void ATutorialWall::OnWaveCleared(int32 WaveNumber)
{
	if (WaveNumber == LowersAtWaveNumber)
	{
		Destroy();
	}
}






