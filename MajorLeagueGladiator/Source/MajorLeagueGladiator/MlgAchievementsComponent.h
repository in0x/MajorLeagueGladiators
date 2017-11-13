// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "MlgAchievementsComponent.generated.h"

UCLASS(Config=Game)
class MAJORLEAGUEGLADIATOR_API UMlgAchievementsComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UMlgAchievementsComponent();

	virtual void BeginPlay() override;

	void OnWaveClearedCheckProgress(int32 WaveNumber);

private:
	UPROPERTY(Config)
	int32 easyAchievementWave;

	UPROPERTY(Config)
	int32 mediumAchievementWave;

	UPROPERTY(Config)
	int32 hardAchievementWave;

	UPROPERTY(Config)
	FString beatEasyAchievementId;

	UPROPERTY(Config)
	FString beatMediumAchievementId;

	UPROPERTY(Config)
	FString beatHardAchievementId;	
};
