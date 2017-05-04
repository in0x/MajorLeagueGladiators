// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Engine/StaticMeshActor.h"
#include "TutorialWallDisappearHealth.generated.h"

class UHealthComponent;

UCLASS()
class MAJORLEAGUEGLADIATOR_API ATutorialWallDisappearHealth : public AStaticMeshActor
{
	GENERATED_BODY()
	
public:
	virtual void BeginPlay() override;
	
private:
	UHealthComponent* cachedPlayerHealthComponent;

	UFUNCTION()
	void OnPlayerHealthChanged(float NewHealthPercentage, float OldHealthPercentage);
};
