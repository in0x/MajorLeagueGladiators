// Fill out your copyright notice in the Description page of Project Settings.

#include "MajorLeagueGladiator.h"
#include "MlgGameState.h"
#include "ReplicatedEffectsComponent.h"
#include "WaveSystem/WaveSystemComponent.h"
#include "MlgGameInstance.h"

AMlgGameState::AMlgGameState(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	waveSystemComponent = ObjectInitializer.CreateDefaultSubobject<UWaveSystemComponent>(this, TEXT("WaveSystemComponent"));
	replicatedEffectsComponent = ObjectInitializer.CreateDefaultSubobject<UReplicatedEffectsComponent>(this, TEXT("ReplicatedEffectsComponent"));
}

void AMlgGameState::BeginPlay() 
{
	if (!GEngine->IsEditor())
	{
		UGameInstance* gameInstance = GetGameInstance();
		UMlgGameInstance* mlgGameInstance = CastChecked<UMlgGameInstance>(gameInstance);
		mlgGameInstance->QueryAchievements();

		waveSystemComponent->OnWaveCleared.AddUObject(this, &AMlgGameState::WaveCleared);
	}
}

void AMlgGameState::WaveCleared(int32 WaveNumber)
{	
	const FString* achId = nullptr;

	if (WaveNumber == easyAchievementWave)
	{
		float newAchValue = 0.f;
		achId = &beatEasyAchievementId;
	}

	if (WaveNumber == mediumAchievementWave)
	{
		float newAchValue = 0.f;
		achId = &beatMediumAchievementId;
	}

	if (WaveNumber == hardAchievementWave)
	{
		float newAchValue = 0.f;
		achId = &beatHardAchievementId;
	}

	if (nullptr != achId)
	{
		UGameInstance* gameInstance = GetGameInstance();
		UMlgGameInstance* mlgGameInstance = CastChecked<UMlgGameInstance>(gameInstance);
		mlgGameInstance->WriteAchievement(*achId, 1.f);
	}
}


