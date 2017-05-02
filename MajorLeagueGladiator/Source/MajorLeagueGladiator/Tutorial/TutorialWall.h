// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Engine/StaticMeshActor.h"
#include "TutorialWall.generated.h"

/**
 * 
 */
UCLASS()
class MAJORLEAGUEGLADIATOR_API ATutorialWall : public AStaticMeshActor
{
	GENERATED_BODY()
	
public:
	virtual void BeginPlay() override;
	
private:
	UPROPERTY(EditAnywhere)
	int32 LowersAtWaveNumber;

	void OnWaveCleared(int32 WaveNumber);
};
