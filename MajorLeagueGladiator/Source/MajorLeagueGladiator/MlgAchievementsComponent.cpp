// Fill out your copyright notice in the Description page of Project Settings.

#include "MajorLeagueGladiator.h"
#include "MlgAchievementsComponent.h"
#include "MlgGameInstance.h"

UMlgAchievementsComponent::UMlgAchievementsComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UMlgAchievementsComponent::BeginPlay()
{
	Super::BeginPlay();

	if (!GEngine->IsEditor())
	{
		UGameInstance* gameInstance = UGameplayStatics::GetGameInstance(GetWorld());
		UMlgGameInstance* mlgGameInstance = CastChecked<UMlgGameInstance>(gameInstance);
		mlgGameInstance->QueryAchievements();
	}
}

void UMlgAchievementsComponent::OnWaveClearedCheckProgress(int32 WaveNumber)
{
	if (GEngine->IsEditor())
	{
		return;
	}
	
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
		UGameInstance* gameInstance = UGameplayStatics::GetGameInstance(GetWorld());
		UMlgGameInstance* mlgGameInstance = CastChecked<UMlgGameInstance>(gameInstance);
		mlgGameInstance->WriteAchievement(*achId, 1.f);
	}
}



