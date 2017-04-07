// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Blueprint/UserWidget.h"
#include "PlayerHudWidget.generated.h"

UCLASS()
class MAJORLEAGUEGLADIATOR_API UPlayerHudWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	UFUNCTION()
	void SetCurrentPercentage(float newHealthPercentage, float oldHealthPercentage);

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Hud")
	float CurrentPercentage = 1;
};